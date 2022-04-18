const express = require('express');
const { createServer } = require('http');
const { Server } = require('socket.io');
const { spawn } = require('child_process');
const app = express();
const httpServer = createServer(app);
const io = new Server(httpServer);
const child = spawn('main.exe');
const port = 3000;

app.use(express.static('public'));

child.stdout.on('data', (data) => {
    io.emit('result', data.toString());
});

child.on('exit', (code, signal) => {
    if(code) console.log(`Process exit with code ${code}`);
    if(signal) console.log(`Process exit with signal ${signal}`);
});

io.on('connection', (socket) => {
    console.log('user connected');
    socket.on('command', (data) => {
        child.stdin.write(data);
    })
});



httpServer.listen(3000);