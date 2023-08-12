#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

// Define the Huffman tree node structure
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Function to build the Huffman tree
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    auto compare = [](HuffmanNode* a, HuffmanNode* b) { return a->freq > b->freq; };
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, decltype(compare)> pq(compare);

    for (const auto& entry : freqMap) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Function to build Huffman codes
void buildHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr)
        return;

    if (root->data != '\0')
        huffmanCodes[root->data] = code;

    buildHuffmanCodes(root->left, code + "0", huffmanCodes);
    buildHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Compress a text file using Huffman coding
void compressFile(const string& inputFile, const string& outputFile) {
    // Read the input file and calculate character frequencies
    unordered_map<char, int> freqMap;
    ifstream input(inputFile);
    char ch;
    while (input.get(ch)) {
        freqMap[ch]++;
    }
    input.close();

    // Build Huffman tree and codes
    HuffmanNode* root = buildHuffmanTree(freqMap);
    unordered_map<char, string> huffmanCodes;
    buildHuffmanCodes(root, "", huffmanCodes);

    // Write the Huffman codes and compressed data to the output file
    ofstream output(outputFile, ios::binary);
    output << freqMap.size() << '\n';  // Write the number of distinct characters

    for (const auto& entry : freqMap) {
        output << entry.first << ' ' << entry.second << '\n';
    }

    ifstream inputAgain(inputFile);
    string compressedData = "";
    while (inputAgain.get(ch)) {
        compressedData += huffmanCodes[ch];
    }
    inputAgain.close();

    // Convert binary string to bytes and write to output file
    while (compressedData.size() % 8 != 0) {
        compressedData += '0';  // Padding
    }

    for (size_t i = 0; i < compressedData.size(); i += 8) {
        string byteStr = compressedData.substr(i, 8);
        char byte = static_cast<char>(stoi(byteStr, nullptr, 2));
        output.put(byte);
    }

    output.close();

    cout << "File compressed successfully.\n";
}

void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile);

    int numDistinctChars;
    input >> numDistinctChars;
    input.ignore(); // Ignore newline

    unordered_map<char, int> freqMap;
    for (int i = 0; i < numDistinctChars; ++i) {
        char ch;
        int freq;
        input >> ch >> freq;
        freqMap[ch] = freq;
        input.ignore(); // Ignore newline
    }

    HuffmanNode* root = buildHuffmanTree(freqMap);
    HuffmanNode* currentNode = root;

    char byte;
    while (input.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            currentNode = bit ? currentNode->right : currentNode->left;

            if (currentNode->left == nullptr && currentNode->right == nullptr) {
                output.put(currentNode->data);
                currentNode = root;
            }
        }
    }

    input.close();
    output.close();
    cout << "File decompressed successfully.\n";
}

int main() {
    int choice;
    cout << "Choose an option:\n";
    cout << "1. Compress\n";
    cout << "2. Decompress\n";
    cin >> choice;

    if (choice == 1) {
        string inputFile, outputFile;
        cout << "Enter the input file name: ";
        cin >> inputFile;
        cout << "Enter the output file name: ";
        cin >> outputFile;
        compressFile(inputFile, outputFile);
    } else if (choice == 2) {
        string inputFile, outputFile;
        cout << "Enter the input file name: ";
        cin >> inputFile;
        cout << "Enter the output file name: ";
        cin >> outputFile;
        decompressFile(inputFile, outputFile);
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
