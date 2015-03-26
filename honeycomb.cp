#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

struct TrieNode{
    
    char alphabet;
    bool valid;                     // Indicate whether this is a valid word
    
    TrieNode *next[26];
    TrieNode (char _alphabet = 0, bool _valid = false) {
        
        alphabet = _alphabet;
        valid = _valid;
        for (int i=0; i<=25; i++)
            next[i] = NULL;
    }
};


// Build trie from dictionary
TrieNode* buildTrie(const vector<string> &dict) {

    TrieNode *root = new TrieNode, *curr;
    
    if (dict.size() == 0)
        return root;
    
    for (int i=0; i<=dict.size()-1; i++) {
        curr = root;
        for (int j=0; j<=dict[i].length()-1; j++) {
            if (!curr->next[ dict[i][j]-'A' ]) {
                TrieNode *new_node = new TrieNode(dict[i][j]);
                curr->next[ dict[i][j]-'A' ] = new_node;
            }
            curr = curr->next[ dict[i][j]-'A' ];
        }
        curr->valid = true;
    }
    return root;
}


// Determine if two cells are adjacent
bool isAdjCell(int curr_layer,
               int curr_cell,
               int next_layer,
               int next_cell) {
    
    if (curr_layer == 0 && curr_cell != 0)      // Layer 0 with nonzero index
        return false;
    
    if (next_layer == 0 && next_cell != 0)      // Layer 0 with nonzero index
        return false;
    
    if ( ( curr_cell <= -1 || curr_cell >= curr_layer*6 ) &&
        curr_layer != 0)                        // Index is out-of-bound
        return false;
    
    if ( ( next_cell <= -1 || next_cell >= next_layer*6 ) &&
        next_layer != 0)                        // Index is out-of-bound
        return false;
    
    if (abs(curr_layer - next_layer) >= 2)      // Diff of layers >= 2
        return false;
    
    if (curr_layer == next_layer) {             // Two indices belong to the same layer
        if (abs(curr_cell - next_cell) == 1 || abs(curr_cell - next_cell) == 6*curr_layer-1)
            return true;                        // Diff of indices == 1
        else return false;
    }
    
    int inner_cell, outer_cell, inner_layer, outer_layer;
    
    if (curr_layer > next_layer) {
        inner_cell = next_cell;
        inner_layer = next_layer;
        outer_cell = curr_cell;
        outer_layer = curr_layer;
    }
    else {
        inner_cell = curr_cell;
        inner_layer = curr_layer;
        outer_cell = next_cell;
        outer_layer = next_layer;
    }
    
    if (inner_layer == 0)                       // Layer 0 is adjacent to all nodes in layer 1
        return true;
    
    int inner_edge, inner_pos, outer_edge, outer_pos;
    bool inner_is_corner = false, outer_is_corner = false;
    
    if (inner_layer == 1) {
        inner_edge = inner_cell / inner_layer;
        inner_pos = 0;
    }
    else {
        inner_edge = inner_cell / inner_layer;
        inner_pos = inner_cell % inner_layer;
    }
    
    outer_edge = outer_cell / outer_layer;
    outer_pos = outer_cell % outer_layer;
    
    if (inner_pos == 0)
        inner_is_corner = true;
    
    if (outer_pos == 0)
        outer_is_corner = true;
    
    if (abs( inner_edge-outer_edge ) >= 2) {    // Indices are located apart from more than 1 edge

        if ( abs( inner_edge-outer_edge ) == 5 &&
            inner_cell == 0 && outer_cell == 6*outer_layer-1)
            return true;
        else return false;
    }
    else {
    
        if (inner_edge == outer_edge) {
            if (inner_pos == outer_pos || inner_pos+1 == outer_pos)
                return true;                    // Indices are on the same edge and adjacent to each other
            else return false;
        }
        else {
            if (inner_is_corner && inner_pos == 0 &&
                outer_pos == outer_layer-1 &&
                inner_edge-1 == outer_edge)     // Indices are on the diff edges but are end points
                return true;
            else return false;
            
        }
    }
}

// Traverse from curr_layer and curr_cell and store all valid strings into set<string>
set<string> traverseComb(const vector<string> &comb,
                         TrieNode* trie_root,
                         map<pair<int, int>, bool> visited,
                         string curr_path,
                         int curr_layer,
                         int curr_cell,
                         int max_layer) {
    
    static set<string> result;

    if (!trie_root->next[comb[curr_layer][curr_cell]-'A'])
        return result;                                   // Return if there's no corresponding char in trie
    
    else {
        
        visited[make_pair(curr_layer, curr_cell)] = true;
        trie_root = trie_root->next[ comb[curr_layer][curr_cell]-'A' ];
        curr_path += comb[curr_layer][curr_cell];
        
        if (trie_root->valid && result.find(curr_path) == result.end())
            result.insert(curr_path);                   // Insert the word if still not exist in the set
    }
    
    if (curr_layer == 0) {
        
        if (max_layer == 0)
            return result;
        
        else {
            for (int i=0; i<=5; i++) {
                if (!visited[make_pair(1, i)])
                    traverseComb(comb, trie_root, visited, curr_path, 1, i, max_layer);
            }
        }
        return result;
    }
    
    else {      // Current layer is not 0
        
        for (int i=0; i<=curr_layer*6-1; i++) {

            if ( !visited[make_pair(curr_layer, i)] &&       // Traverse the node with the same layer
                isAdjCell(curr_layer, curr_cell, curr_layer, i) )
                traverseComb(comb, trie_root, visited, curr_path, curr_layer, i, max_layer);
        }
        
        if (curr_layer == 1) {

            if (!visited[make_pair(0, 0)])                  // Traverse the inner layer when curr layer = 1
                traverseComb(comb, trie_root, visited, curr_path, 0, 0, max_layer);
        }
        
        else {      // Current layer is not 1
            for (int i=0; i<=(curr_layer-1)*6-1; i++) {

                if (!visited[make_pair(curr_layer-1, i)]&&  // Traverse the inner layer when curr layer != 1
                    isAdjCell(curr_layer, curr_cell, curr_layer-1, i))
                    traverseComb(comb, trie_root, visited, curr_path, curr_layer-1, i, max_layer);
            }
        
        }
        
        if (curr_layer < max_layer) {
            for (int i=0; i<=(curr_layer+1)*6-1; i++) {

                if (!visited[make_pair(curr_layer+1, i)] &&  // Traverse the outer layer
                    isAdjCell(curr_layer, curr_cell, curr_layer+1, i))
                    traverseComb(comb, trie_root, visited, curr_path, curr_layer+1, i, max_layer);
            }
        
        }
    }
    return result;
}

// Start from different cells and call traverseComb iteratively
set<string> findValidWord(const vector<string> &input,
                          const vector<string> &dict) {
    
    set<string> all_valid_words, buffer;
    set<string>::iterator it;
    TrieNode *root = buildTrie(dict), *curr = NULL;
    string candidate;

    for (int i=0; i<=input.size()-1; i++) {
        for (int j=0; j<=input[i].length()-1; j++) {
            curr = root;
            map<pair<int, int>, bool> hmap;
            buffer = traverseComb(input, curr, hmap, candidate, i, j, input.size()-1);
            for (it=buffer.begin(); it!=buffer.end(); it++) {
                if (all_valid_words.find(*it) == all_valid_words.end())
                    all_valid_words.insert(*it);
            }
        }
    }
    return all_valid_words;
}

// Helper function to remove the first row in the honeycomb.txt
bool isCap(char c) {
    return (c-'A' >= 0) && (c-'A' <= 25);
}

// Read the strings from input file and store it into vector<string> output
void readFile(istream &input,
              vector<string> &output) {
    
    string raw_text;
    while (getline(input, raw_text))
        if (isCap(raw_text[0]))
            output.push_back(raw_text);

}

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        
        cout << "Please execute this program in the following format:\n";
        cout << "./a.out honeycomb.txt dictionary.txt\n";
        return 1;
    }
    
    ifstream input_file(argv[1]), dict_file(argv[2]);
    
    if (!input_file.is_open() || !dict_file.is_open()) {
        
        cout << "File not found!\n";
        return 1;
    }
    
    vector<string> input, dict;
    set<string> final_result;
    set<string>::iterator it;
    
    readFile(input_file, input);
    input_file.close();
    readFile(dict_file, dict);
    dict_file.close();
    
    final_result = findValidWord(input, dict);
    
    for (it=final_result.begin(); it!=final_result.end(); it++)
        cout << *it << endl;

    return 0;
}

