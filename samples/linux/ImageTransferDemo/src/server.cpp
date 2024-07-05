#include <iostream>
#include <rdk/core.h>
#include <opencv2/opencv.hpp>



void Session(std::shared_ptr<TCPPort> client)
{
    std::cout << "有新客户端连接服务器" << std::endl;

    std::shared_ptr<BinaryTransfer> transfer = std::make_shared<BinaryTransfer>(client);

    cv::VideoCapture vc(0);
    if (!vc.isOpened()) {
        std::cout << "camera cannot open" << std::endl;
        return ;
    }

    std::cout << "ok" << std::endl;

    cv::Mat frame;
    while (true) {
        vc >> frame;
        
        if (frame.empty()) continue;
        cv::Mat img;
        cv::resize(frame, img, cv::Size(256, 256));

        img.convertTo(img, CV_8UC3);

	    int img_length = img.total() * img.channels();
	    char* image_array_ptr = new char[img_length];
	    memcpy(image_array_ptr, img.ptr<char>(0), img_length * sizeof(char));

	    // 从float* 再转换为cv::Mat，验证转换是否正确
	    cv::Mat a = cv::Mat(256, 256, CV_8UC3, image_array_ptr);

        transfer->send_binary(reinterpret_cast<uint8_t*>(image_array_ptr), img_length);

    }
    
    client->close();
    std::cout << "服务器主动断开连接" << std::endl;
}

int main()
{
    std::shared_ptr<TCPPort> server = std::make_shared<TCPPort>();
    server->set_port(7788); //设置服务器端口，并开启监听

    while (true) {
        auto client = server->accept(); //接受一个客户端连接请求
        std::thread th(Session, client); //启动一个线程处理 服务器与客户端 的交互
        th.join();
    }

    return 0;
}