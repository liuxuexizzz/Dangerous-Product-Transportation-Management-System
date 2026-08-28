package com.js.freightSafety.controller;

import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

@Slf4j
@RestController
@RequestMapping("video")
public class VideoController {
    // 标识图片（帧）的开始和结尾
    private static final byte[] prefix = "Frame Begin".getBytes();
    private static final byte[] suffix = "Frame Over".getBytes();
    // 一个包的大小为 1430 byte
    private static final int bufferSize = 1430;
    // 视频文件保存路径
    private static final String videoFilePath = "zheshiceshi";

    private static final List<Socket> connectionList = new ArrayList<>();

    {
        new Thread(this::startServer).start();
    }

    private void startServer() {
        ServerSocket serverSocket;

        try {
            serverSocket = new ServerSocket(9091);
            log.info("Server started on port 9091");

            while (true) {
                Socket clientSocket = serverSocket.accept();
                connectionList.add(clientSocket);
                log.info("Connected: " + clientSocket.getInetAddress());

                Thread clientThread = new Thread(() -> handleSocket(clientSocket));
                clientThread.start();
            }
        } catch (IOException e) {
            log.error("Error starting server", e);
        }
    }

    private static void handleSocket(Socket socket) {
        byte[] tmp = new byte[0];
        long t1 = System.currentTimeMillis();
        try (DataInputStream dis = new DataInputStream(socket.getInputStream())) {
            FileOutputStream videoOutputStream = new FileOutputStream(videoFilePath);
            System.out.println(videoOutputStream);
            while (true) {
                byte[] data = new byte[bufferSize];
                // 读取一张图片
//                dis.readFully(data);
                dis.read(data);
                System.out.println(new String(data));
                if (startsWith(data, prefix)) {
                    // 移除 prefix
                    data = copyOfRange(data, prefix.length, data.length);
                    // 读取数据
                    while (!endsWith(tmp, suffix)) {
                        tmp = append(tmp, data);
                        dis.readFully(data);
                    }
                    // 读取尾部的数据
                    tmp = append(tmp, copyOfRange(data, 0, data.length - suffix.length));
                    // 保存帧数据到视频文件
                    videoOutputStream.write(tmp);


                    long t2 = System.currentTimeMillis();
                    int fps = 1000 / (int) (t2 - t1);
                    System.out.println("FPS: " + fps);

                    tmp = new byte[0];
                    t1 = t2;
                } else {

                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static boolean startsWith(byte[] data, byte[] prefix) {
        if (data.length < prefix.length) {
            log.error("图片数据错误");
            return false;
        }
        for (int i = 0; i < prefix.length; i++) {
            if (data[i] != prefix[i]) {
                return false;
            }
        }
        return true;
    }

    private static boolean endsWith(byte[] data, byte[] suffix) {
        if (data.length < suffix.length) {
            log.error("图片数据错误");
            return false;
        }
        for (int i = 0; i < suffix.length; i++) {
            if (data[data.length - suffix.length + i] != suffix[i]) {
                return false;
            }
        }
        return true;
    }

    private static byte[] append(byte[] array1, byte[] array2) {
        byte[] result = new byte[array1.length + array2.length];
        System.arraycopy(array1, 0, result, 0, array1.length);
        System.arraycopy(array2, 0, result, array1.length, array2.length);
        return result;
    }

    private static byte[] copyOfRange(byte[] original, int from, int to) {
        int length = to - from;
        byte[] copy = new byte[length];
        System.arraycopy(original, from, copy, 0, Math.min(length, original.length - from));
        return copy;
    }

}
