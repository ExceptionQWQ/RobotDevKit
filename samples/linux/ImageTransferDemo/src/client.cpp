#include <iostream>
#include <rdk/core.h>
#include <opencv2/opencv.hpp>

int main()
{
    //连接本机服务器
    std::shared_ptr<TCPPort> client = std::make_shared<TCPPort>();
    if (client->connect("127.0.0.1", 7788)) {
        std::cout << "连接成功" << std::endl;
    } else {
        std::cout << "连接失败" << std::endl;
        return -1;
    }

    std::shared_ptr<BinaryTransfer> transfer = std::make_shared<BinaryTransfer>(client);

    int img_length = 256 * 256 * 3;
    char* recv_buff = new char[img_length + 32];
    while (true) {
        std::size_t recv_bytes = transfer->recv_binary(reinterpret_cast<uint8_t*>(recv_buff), img_length);
        printf("recv_bytes:%d\n", recv_bytes);

        if (recv_bytes != img_length) continue;

        cv::Mat img = cv::Mat(256, 256, CV_8UC3, recv_buff);
        cv::imshow("img", img);
        cv::waitKey(1);

    }

    return 0;
}