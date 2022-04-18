#include <bits/stdc++.h>
#include "misc.h"

#define KING(c) ((c) ? bking : wking)
#define IS_KING(p) (((p) == wking) || ((p) == bking))

enum colors
{
  white,
  black
};
enum pieces
{
  blank = 0,
  wking,
  wqueen,
  wrook,
  wbishop,
  wknight,
  wpawn,
  bking,
  bqueen,
  brook,
  bbishop,
  bknight,
  bpawn
};

typedef struct DirtyPiece
{
  int dirtyNum;
  int pc[3];
  int from[3];
  int to[3];
} DirtyPiece;

typedef struct Accumulator
{
  alignas(64) short accumulation[2][256];
  int computedAccumulation;
} Accumulator;

typedef struct NNdata
{
  Accumulator accumulator;
  DirtyPiece dirtyPiece;
} NNdata;

typedef struct Position
{
  int player;
  int *pieces;
  int *squares;
  NNdata *nn[3];
} Position;

enum
{
  PS_W_PAWN = 1,
  PS_B_PAWN = 1 * 64 + 1,
  PS_W_KNIGHT = 2 * 64 + 1,
  PS_B_KNIGHT = 3 * 64 + 1,
  PS_W_BISHOP = 4 * 64 + 1,
  PS_B_BISHOP = 5 * 64 + 1,
  PS_W_ROOK = 6 * 64 + 1,
  PS_B_ROOK = 7 * 64 + 1,
  PS_W_QUEEN = 8 * 64 + 1,
  PS_B_QUEEN = 9 * 64 + 1,
  PS_END = 10 * 64 + 1
};

uint32_t PieceToIndex[2][14] = {
    {0, 0, PS_W_QUEEN, PS_W_ROOK, PS_W_BISHOP, PS_W_KNIGHT, PS_W_PAWN,
     0, PS_B_QUEEN, PS_B_ROOK, PS_B_BISHOP, PS_B_KNIGHT, PS_B_PAWN, 0},
    {0, 0, PS_B_QUEEN, PS_B_ROOK, PS_B_BISHOP, PS_B_KNIGHT, PS_B_PAWN,
     0, PS_W_QUEEN, PS_W_ROOK, PS_W_BISHOP, PS_W_KNIGHT, PS_W_PAWN, 0}};

static const uint32_t NnueVersion = 0x7AF32F16u;
enum
{
  FV_SCALE = 16,
};

enum
{
  kHalfDimensions = 256,
  FtInDims = 64 * PS_END,
  FtOutDims = kHalfDimensions * 2
};

typedef uint8_t mask_t;
typedef uint32_t mask2_t;

typedef int8_t clipped_t;
typedef int8_t weight_t;

typedef struct
{
  size_t size;
  unsigned values[30];
} IndexList;

int orient(int c, int s)
{
  return s ^ (c == white ? 0x00 : 0x3f);
}

unsigned make_index(int c, int s, int pc, int ksq)
{
  return orient(c, s) + PieceToIndex[c][pc] + PS_END * ksq;
}

static void half_kp_append_active_indices(const Position *pos, const int c,
                                          IndexList *active)
{
  int ksq = pos->squares[c];
  ksq = orient(c, ksq);
  for (int i = 2; pos->pieces[i]; i++)
  {
    int sq = pos->squares[i];
    int pc = pos->pieces[i];
    active->values[active->size++] = make_index(c, sq, pc, ksq);
  }
}

static void half_kp_append_changed_indices(const Position *pos, const int c,
                                           const DirtyPiece *dp, IndexList *removed, IndexList *added)
{
  int ksq = pos->squares[c];
  ksq = orient(c, ksq);
  for (int i = 0; i < dp->dirtyNum; i++)
  {
    int pc = dp->pc[i];
    if (IS_KING(pc))
      continue;
    if (dp->from[i] != 64)
      removed->values[removed->size++] = make_index(c, dp->from[i], pc, ksq);
    if (dp->to[i] != 64)
      added->values[added->size++] = make_index(c, dp->to[i], pc, ksq);
  }
}

static void append_active_indices(const Position *pos, IndexList active[2])
{
  for (unsigned c = 0; c < 2; c++)
    half_kp_append_active_indices(pos, c, &active[c]);
}

static void append_changed_indices(const Position *pos, IndexList removed[2],
                                   IndexList added[2], bool reset[2])
{
  const DirtyPiece *dp = &(pos->nn[0]->dirtyPiece);

  if (pos->nn[1]->accumulator.computedAccumulation)
  {
    for (unsigned c = 0; c < 2; c++)
    {
      reset[c] = dp->pc[0] == (int)KING(c);
      if (reset[c])
        half_kp_append_active_indices(pos, c, &added[c]);
      else
        half_kp_append_changed_indices(pos, c, dp, &removed[c], &added[c]);
    }
  }
  else
  {
    const DirtyPiece *dp2 = &(pos->nn[1]->dirtyPiece);
    for (unsigned c = 0; c < 2; c++)
    {
      reset[c] = dp->pc[0] == (int)KING(c) || dp2->pc[0] == (int)KING(c);
      if (reset[c])
        half_kp_append_active_indices(pos, c, &added[c]);
      else
      {
        half_kp_append_changed_indices(pos, c, dp, &removed[c], &added[c]);
        half_kp_append_changed_indices(pos, c, dp2, &removed[c], &added[c]);
      }
    }
  }
}

static weight_t hidden1_weights alignas(64)[32 * 512];
static weight_t hidden2_weights alignas(64)[32 * 32];
static weight_t output_weights alignas(64)[1 * 32];

static int32_t hidden1_biases alignas(64)[32];
static int32_t hidden2_biases alignas(64)[32];
static int32_t output_biases[1];

int32_t affine_propagate(clipped_t *input, int32_t *biases,
                         weight_t *weights)
{
  int32_t sum = biases[0];
  for (unsigned j = 0; j < 32; j++)
    sum += weights[j] * input[j];
  return sum;
}

void affine_txfm(clipped_t *input, void *output, unsigned inDims,
                 unsigned outDims, int32_t *biases, const weight_t *weights,
                 mask_t *inMask, mask_t *outMask, const bool pack8_and_calc_mask)
{

  int32_t tmp[outDims];

  for (unsigned i = 0; i < outDims; i++)
    tmp[i] = biases[i];

  for (unsigned idx = 0; idx < inDims; idx++)
    if (input[idx])
      for (unsigned i = 0; i < outDims; i++)
        tmp[i] += (int8_t)input[idx] * weights[outDims * idx + i];

  clipped_t *outVec = (clipped_t *)output;
  for (unsigned i = 0; i < outDims; i++)
    outVec[i] = clamp(tmp[i] >> 6, 0, 127);
}

// Input feature converter
static short ft_biases alignas(64)[kHalfDimensions];
static short ft_weights alignas(64)[kHalfDimensions * FtInDims];

// Calculate cumulative value without using difference calculation
void refresh_accumulator(Position *pos)
{
  Accumulator *accumulator = &(pos->nn[0]->accumulator);

  IndexList activeIndices[2];
  activeIndices[0].size = activeIndices[1].size = 0;
  append_active_indices(pos, activeIndices);

  for (unsigned c = 0; c < 2; c++)
  {
    memcpy(accumulator->accumulation[c], ft_biases,
           kHalfDimensions * sizeof(short));

    for (size_t k = 0; k < activeIndices[c].size; k++)
    {
      unsigned index = activeIndices[c].values[k];
      unsigned offset = kHalfDimensions * index;

      for (unsigned j = 0; j < kHalfDimensions; j++)
        accumulator->accumulation[c][j] += ft_weights[offset + j];
    }
  }

  accumulator->computedAccumulation = 1;
}

// Calculate cumulative value using difference calculation if possible
bool update_accumulator(Position *pos)
{
  Accumulator *accumulator = &(pos->nn[0]->accumulator);
  if (accumulator->computedAccumulation)
    return true;

  Accumulator *prevAcc;
  if ((!pos->nn[1] || !(prevAcc = &pos->nn[1]->accumulator)->computedAccumulation) && (!pos->nn[2] || !(prevAcc = &pos->nn[2]->accumulator)->computedAccumulation))
    return false;

  IndexList removed_indices[2], added_indices[2];
  removed_indices[0].size = removed_indices[1].size = 0;
  added_indices[0].size = added_indices[1].size = 0;
  bool reset[2];
  append_changed_indices(pos, removed_indices, added_indices, reset);

  for (unsigned c = 0; c < 2; c++)
  {
    if (reset[c])
    {
      memcpy(accumulator->accumulation[c], ft_biases,
             kHalfDimensions * sizeof(short));
    }
    else
    {
      memcpy(accumulator->accumulation[c], prevAcc->accumulation[c],
             kHalfDimensions * sizeof(short));
      // Difference calculation for the deactivated features
      for (unsigned k = 0; k < removed_indices[c].size; k++)
      {
        unsigned index = removed_indices[c].values[k];
        const unsigned offset = kHalfDimensions * index;

        for (unsigned j = 0; j < kHalfDimensions; j++)
          accumulator->accumulation[c][j] -= ft_weights[offset + j];
      }
    }

    // Difference calculation for the activated features
    for (unsigned k = 0; k < added_indices[c].size; k++)
    {
      unsigned index = added_indices[c].values[k];
      const unsigned offset = kHalfDimensions * index;

      for (unsigned j = 0; j < kHalfDimensions; j++)
        accumulator->accumulation[c][j] += ft_weights[offset + j];
    }
  }

  accumulator->computedAccumulation = 1;
  return true;
}

// Convert input features
void transform(Position *pos, clipped_t *output, mask_t *outMask)
{
  if (!update_accumulator(pos))
    refresh_accumulator(pos);

  short(*accumulation)[2][256] = &pos->nn[0]->accumulator.accumulation;

  const int perspectives[2] = {pos->player, !pos->player};
  for (unsigned p = 0; p < 2; p++)
  {
    const unsigned offset = kHalfDimensions * p;

    for (unsigned i = 0; i < kHalfDimensions; i++)
    {
      short sum = (*accumulation)[perspectives[p]][i];
      output[offset + i] = clamp(sum, 0, 127);
    }
  }
}

struct NetData
{
  alignas(64) clipped_t input[FtOutDims];
  clipped_t hidden1_out[32];
  int8_t hidden2_out[32];
};

// Evaluation function
int nn_evaluate_pos(Position *pos)
{
  int32_t out_value;
  alignas(8) mask_t input_mask[FtOutDims / (8 * sizeof(mask_t))];
  alignas(8) mask_t hidden1_mask[8 / sizeof(mask_t)] = {0};

  struct NetData buf;
  #define B(x) (buf.x)

  transform(pos, B(input), input_mask);

  affine_txfm(B(input), B(hidden1_out), FtOutDims, 32,
              hidden1_biases, hidden1_weights, input_mask, hidden1_mask, true);

  affine_txfm(B(hidden1_out), B(hidden2_out), 32, 32,
              hidden2_biases, hidden2_weights, hidden1_mask, NULL, false);

  out_value = affine_propagate((int8_t *)B(hidden2_out), output_biases,
                               output_weights);

  return out_value / FV_SCALE;
}

static void read_output_weights(weight_t *w, const char *d)
{
  for (unsigned i = 0; i < 32; i++)
  {
    unsigned c = i;
    w[c] = *d++;
  }
}

unsigned wt_idx(unsigned r, unsigned c, unsigned dims)
{
  (void)dims;
  return c * 32 + r;
}

static const char *read_hidden_weights(weight_t *w, unsigned dims,
                                       const char *d)
{
  for (unsigned r = 0; r < 32; r++)
    for (unsigned c = 0; c < dims; c++)
      w[wt_idx(r, c, dims)] = *d++;

  return d;
}

enum
{
  TransformerStart = 3 * 4 + 177,
  NetworkStart = TransformerStart + 4 + 2 * 256 + 2 * 256 * 64 * 641
};

static void init_weights(const void *evalData)
{
  const char *d = (const char *)evalData + TransformerStart + 4;

  // Read transformer
  for (unsigned i = 0; i < kHalfDimensions; i++, d += 2)
    ft_biases[i] = readu_le_u16(d);
  for (unsigned i = 0; i < kHalfDimensions * FtInDims; i++, d += 2)
    ft_weights[i] = readu_le_u16(d);

  // Read network
  d += 4;
  for (unsigned i = 0; i < 32; i++, d += 4)
    hidden1_biases[i] = readu_le_u32(d);
  d = read_hidden_weights(hidden1_weights, 512, d);
  for (unsigned i = 0; i < 32; i++, d += 4)
    hidden2_biases[i] = readu_le_u32(d);
  d = read_hidden_weights(hidden2_weights, 32, d);
  for (unsigned i = 0; i < 1; i++, d += 4)
    output_biases[i] = readu_le_u32(d);
  read_output_weights(output_weights, d);
}

void nn_init(const char *evalFile)
{
  const void *evalData;
  map_t mapping;
  size_t size;

  FD fd = open_file(evalFile);
  evalData = map_file(fd, &mapping);
  size = file_size(fd);
  close_file(fd);
  
  init_weights(evalData);
  if (mapping)
    unmap_file(evalData, mapping);
}

int nn_evaluate(int player, int *pieces, int *squares) {
  NNdata nn;
  nn.accumulator.computedAccumulation = 0;

  Position pos;
  pos.nn[0] = &nn;
  pos.nn[1] = 0;
  pos.nn[2] = 0;
  pos.player = player;
  pos.pieces = pieces;
  pos.squares = squares;
  return nn_evaluate_pos(&pos);
}

int nn_evaluate_fen(const char *fen) {
  int pieces[33], squares[33], player, castle, fifty, move_number;
  decode_fen((char *)fen, &player, &castle, &fifty, &move_number, pieces, squares);
  return nn_evaluate(player, pieces, squares);
}