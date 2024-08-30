#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
using namespace std::chrono;

struct Node {
    string word;
    string part_of_speech;
    string meaning;
    Node* left;
    Node* right;

    Node(string w, string pos, string m) : word(w), part_of_speech(pos), meaning(m), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;
    int wordCount;

    void _insert(Node*& current, string word, string part_of_speech, string meaning) {
        if (!current) {
            current = new Node(word, part_of_speech, meaning);
            wordCount++;
        }
        else if (word < current->word) {
            _insert(current->left, word, part_of_speech, meaning);
        }
        else if (word > current->word) {
            _insert(current->right, word, part_of_speech, meaning);
        }
        else {
            cout << "Word already exists in the dictionary." << endl;
        }
    }

    Node* _search(Node* current, string word) {
        if (!current) {
            return nullptr;
        }
        else if (word == current->word) {
            return current;
        }
        else if (word < current->word) {
            return _search(current->left, word);
        }
        else {
            return _search(current->right, word);
        }
    }

public:
    BST() : root(nullptr), wordCount(0) {}

    void insert(string word, string part_of_speech, string meaning) {
        _insert(root, word, part_of_speech, meaning);
    }

    Node* search(string word) {
        return _search(root, word);
    }

    int getWordCount() {
        return wordCount;
    }
};

int main() {
    BST dictionary;
    ifstream file("text.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            
            stringstream ss(line);
            string word, pos, meaning;
            getline(ss, word, '"');
            getline(ss, pos, '"');
            getline(ss, meaning, '"');

            dictionary.insert(word, pos, meaning);
        }
        file.close();
        cout << "Dictionary loaded from text.txt" << endl;
    } else {
        cerr << "Error opening text.txt" << endl;
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "Dictionary");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Text title;
    title.setFont(font);
    title.setString("Dictionary");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::Black);
    title.setPosition(10, 10);

    sf::Text wordCountText;
    wordCountText.setFont(font);
    wordCountText.setCharacterSize(16);
    wordCountText.setFillColor(sf::Color::Black);
    wordCountText.setPosition(10, 50);

    sf::Text inputDescription;
    inputDescription.setFont(font);
    inputDescription.setCharacterSize(16);
    inputDescription.setFillColor(sf::Color::Black);
    inputDescription.setPosition(10, 90);
    inputDescription.setString("Enter a word:");

    sf::RectangleShape inputBox(sf::Vector2f(300, 30));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(1);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setPosition(10, 120);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(16);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(15, 125);

    sf::Text searchResult;
    searchResult.setFont(font);
    searchResult.setCharacterSize(16);
    searchResult.setFillColor(sf::Color::Black);
    searchResult.setPosition(10, 170);

    sf::Text timeTakenText;
    timeTakenText.setFont(font);
    timeTakenText.setCharacterSize(16);
    timeTakenText.setFillColor(sf::Color::Black);
    timeTakenText.setPosition(10, 200);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == '\b') {
                        inputText.setString(inputText.getString().substring(0, inputText.getString().getSize() - 1));
                    }
                    else if (event.text.unicode != '\b' && inputText.getLocalBounds().width < 280) {
                        inputText.setString(inputText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    string word_to_search = inputText.getString().toAnsiString();
                    auto start = high_resolution_clock::now();
                    Node* result = dictionary.search(word_to_search);
                    auto stop = high_resolution_clock::now();
                    if (result) {
                        searchResult.setString("Word: " + result->word + "\nPart of Speech: " + result->part_of_speech + "\nMeaning (Urdu): " + result->meaning);
                    }
                    else {
                        searchResult.setString("Word not found in the dictionary.");
                    }
                    inputText.setString("");

                    auto duration = duration_cast<milliseconds>(stop - start);
                    timeTakenText.setString("Time taken: " + to_string(duration.count()) + " milliseconds");
                }
            }
        }

        wordCountText.setString("Word Count: " + to_string(dictionary.getWordCount()));

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(wordCountText);
        window.draw(inputDescription);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(searchResult);
        window.draw(timeTakenText);
        window.display();
    }

    return 0;
}