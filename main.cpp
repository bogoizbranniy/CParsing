#define CURL_STATICLIB

#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <string>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::vector <std::string> movies;

    int count = 0;
    int pos = 1;

    setlocale(LC_ALL, "rus");

    std::ofstream file;

    std::string new_string;

        curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, "https://www.imdb.com/chart/moviemeter/?sort=rk,asc&mode=simple&page=1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        bool isMovie = false;
        bool isWriteName = false;
        bool isWriteYear = false;
        bool isWriteRate = false;

        for (int i = 3; i < readBuffer.size() - 3; i++) {

            if (count == 0) {
                if (isWriteName) {
                    if (readBuffer[i] == '<') {
                        isWriteName = false;

                        new_string.push_back(' ');

                        new_string = std::to_string(pos) + '.' + new_string;

                        count++;
                        continue;
                    }
                    new_string.push_back(readBuffer[i]);
                }

                if (readBuffer[i] == 'd' && readBuffer[i + 1] == 'i' && readBuffer[i + 2] == 'r' && readBuffer[i + 3] == '.') {
                    isMovie = true;
                }

                if (readBuffer[i] == '>' && isMovie) {
                    isWriteName = true;
                }
            }

            else if (count == 1 && isMovie) {

                if (isWriteYear) {
                    if (readBuffer[i] == ')') {
                        isWriteYear = false;

                        new_string.push_back(')');

                        new_string.push_back('\n');

                        new_string = new_string + "Рейтинг IMDb: ";


                        count = 2;
                        continue;
                    }

                    new_string.push_back(readBuffer[i]);
                }

                if (readBuffer[i] == '(') {
                    new_string.push_back('(');
                    isWriteYear = true;
                }

            }

            else if (count == 2 && isMovie) {

                if (isWriteRate) {
                    if (readBuffer[i] == '<') {
                        isWriteRate = false;
                        isMovie = false;

                        movies.push_back(new_string);

                        new_string.clear();

                        count = 0;

                        pos++;

                        continue;
                    }

                    new_string.push_back(readBuffer[i]);
                }

                if (readBuffer[i - 3] == 'g' && readBuffer[i - 2] == 's' && readBuffer[i - 1] == '"' && readBuffer[i] == '>') {
                    isWriteRate = true;
                }

                if (readBuffer[i] == 'm' && readBuffer[i + 1] == 'n' && readBuffer[i + 2] == '"' && readBuffer[i + 3] == '>') {
                    isMovie = false;

                    isWriteRate = false;

                    count = 0;

                    new_string.clear();
                }

            }
        }

        file.open("relevant.txt");

        for (int i = 0; i < movies.size(); i++) {
            file << movies[i];
            file << '\n';

        }

        file.close();

        readBuffer.clear();

        movies.clear();

        std::cout << "The list has been updated." << std::endl;

    
    return 0;
}
