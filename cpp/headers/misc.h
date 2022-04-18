#include <bits/stdc++.h>
#include <windows.h>

#define bsf(b) __builtin_ctzll(b)
#define bsr(b) (63 - __builtin_clzll(b))

typedef HANDLE FD;
#define FD_ERR INVALID_HANDLE_VALUE
typedef HANDLE map_t;

int readu_le_u32(const void *p)
{
  const uint8_t *q = (const uint8_t*) p;
  return q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
}

short readu_le_u16(const void *p)
{
  const uint8_t *q = (const uint8_t*) p;
  return q[0] | (q[1] << 8);
}

#define clamp(a, b, c) ((a) < (b) ? (b) : (a) > (c) ? (c) : (a))

FD open_file(const char *name)
{
  return CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
      FILE_FLAG_RANDOM_ACCESS, NULL);
}

void close_file(FD fd)
{
  CloseHandle(fd);

}

size_t file_size(FD fd)
{
  DWORD sizeLow, sizeHigh;
  sizeLow = GetFileSize(fd, &sizeHigh);
  return ((uint64_t)sizeHigh << 32) | sizeLow;

}

const void *map_file(FD fd, map_t *map)
{
  DWORD sizeLow, sizeHigh;
  sizeLow = GetFileSize(fd, &sizeHigh);
  *map = CreateFileMapping(fd, NULL, PAGE_READONLY, sizeHigh, sizeLow, NULL);
  if (*map == NULL)
    return NULL;
  return MapViewOfFile(*map, FILE_MAP_READ, 0, 0, 0);
}

void unmap_file(const void *data, map_t map)
{
  if (!data) return;

  UnmapViewOfFile(data);
  CloseHandle(map);

}

/*
FEN
*/
static const char piece_name[] = "_KQRBNPkqrbnp_";
static const char rank_name[] = "12345678";
static const char file_name[] = "abcdefgh";
static const char col_name[] = "WwBb";
static const char cas_name[] = "KQkq";

void decode_fen(const char* fen_str, int* player, int* castle,
       int* fifty, int* move_number, int* piece, int* square)
{
  /*decode fen*/
  int sq,index = 2;
  const char* p = fen_str,*pfen;
  for(int r = 7;r >= 0; r--) {
      for(int f = 0;f <= 7;f++) {
          sq = r * 8 + f;
          if((pfen = strchr(piece_name,*p)) != 0) {
              int pc = (int)(strchr(piece_name,*pfen) - piece_name);
              if(pc == 1) {
                 piece[0] = pc;
                 square[0] = sq;
              } else if(pc == 7) {
                 piece[1] = pc;
                 square[1] = sq;
              } else {
                 piece[index] = pc;
                 square[index] = sq;
                 index++;
              }
          } else if((pfen = strchr(rank_name,*p)) != 0) {
              for(int i = 0;i < pfen - rank_name;i++) {
                  f++;
              }
          } 
          p++;
      }
      p++;
  }
  piece[index] = 0;
  square[index] = 0;

  /*player*/
  if((pfen = strchr(col_name,*p)) != 0)
      *player = ((pfen - col_name) >= 2);
  p++;
  p++;

  /*castling rights*/
  *castle = 0;
  if(*p == '-') {
      p++;
  } else {
      while((pfen = strchr(cas_name,*p)) != 0) {
          *castle |= (1 << (pfen - cas_name));
          p++;
      }
  }
  /*epsquare*/
  int epsquare;
  p++;
  if(*p == '-') {
      epsquare = 0;
      p++;
  } else {
      epsquare = (int)(strchr(file_name,*p) - file_name);
      p++;
      epsquare += 16 * (int)(strchr(rank_name,*p) - rank_name);
      p++;
  }
  square[index] = epsquare;

  /*fifty & hply*/
  p++;
  if(*p && *(p+1) && isdigit(*p) && ( isdigit(*(p+1)) || *(p+1) == ' ' ) ) {
      sscanf(p,"%d %d",fifty,move_number);
      if(*move_number <= 0) *move_number = 1;
  } else {
      *fifty = 0;
      *move_number = 1;
  }
}
