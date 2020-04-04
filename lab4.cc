#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;
void optimal(int);
void fifo(int);
void lru(int);
void clock(int);
void choosePolicy(string,int);
void print_start(string);
void print_end(int);
vector<int> getStream();
int getFarthestIdx(vector<int> frames, vector<int> stream, int start);
int getMostAccessed(unordered_map<int,int> freqMap,vector<int> frames);
int getClockIdx(vector<int>& clockFrames, int pointer);
int main() {
    int no_of_pages;
    string replacementPolicy;
    cin >> no_of_pages;
    cin >> replacementPolicy;
    print_start(replacementPolicy);
    choosePolicy(replacementPolicy,no_of_pages);
    return 0;
}

void print_start(string replacementPolicy)
{
    printf("Replacement Policy = %s\n",replacementPolicy.c_str());
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");
}

void print_end(int no_of_faults)
{
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n",no_of_faults);
}

void choosePolicy(string replacementPolicy, int no_of_pages)
{
    if(replacementPolicy.compare("FIFO") == 0)
        fifo(no_of_pages);
    else if(replacementPolicy.compare("OPTIMAL") == 0)
        optimal(no_of_pages);
    else if(replacementPolicy.compare("LRU") == 0)
        lru(no_of_pages);
    else if(replacementPolicy.compare("CLOCK") == 0)
        clock(no_of_pages);
}

vector<int> getStream()
{
    vector<int> stream;
    int reader = 0;
    while(cin >> reader && reader != -1)
        stream.push_back(reader);
    return stream;
}
string getStringContent(vector<int> frame)
{
    string result;
    char buffer[100];
    for(int i = 0, n = frame.size() ; i < n ; ++i) {
        sprintf(buffer,"%02d ", frame[i]);
        result += buffer;
    }return result;
}
void optimal(int no_of_pages)
{
    vector<int> stream = getStream();
    unordered_map<int,int> frameMap;
    vector<int> frames;
    bool hit = false;
    int page;
    int no_of_faults = 0;
    for(int i = 0, n = stream.size() ; i < n ; ++i)
    {
        page = stream.at(i);
        hit = frameMap.find(page) != frameMap.end();
        if(frames.size() == no_of_pages && !hit) {
            ++no_of_faults;
            // get which idx to remove
            // remove from replace in vector
            // remove from map
            int idx = getFarthestIdx(frames,stream,i);
            frameMap.erase(frames.at(idx));
            frameMap.insert({page,0});
            frames[idx] = page;
            printf("%02d F   %s\n", page, getStringContent(frames).c_str());
        }else
        {
            if(!hit) {
                frames.push_back(page);
                frameMap.insert({page, 0});
            }
            printf("%02d     %s\n",page, getStringContent(frames).c_str());
        }
    }
    print_end(no_of_faults);

}

int getFarthestIdx(vector<int> frames, vector<int> stream, int start)
{
    int streamIdx= start + 1, frameIdx = 0;
    for(int i = 0, n = frames.size() ; i < n ; ++i)
    {
        for(int j = start + 1, m = stream.size() ; j <= m ; ++j)
        {
            if(j == m)
                return i;
            else if(frames.at(i) == stream.at(j)) {
                if (streamIdx < j) {
                    streamIdx = j;
                    frameIdx = i;
                }
                break;
            }
        }
    }
    return frameIdx;
}


void fifo(int no_of_pages)
{
    vector<int> stream = getStream();
    unordered_map<int,int> frameMap;
    vector<int> frames;
    bool hit = false;
    int page;
    int no_of_faults = 0;
    int idx = 0;
    for(int i = 0, n = stream.size() ; i < n ; ++i)
    {
        page = stream.at(i);
        hit = frameMap.find(page) != frameMap.end();
        if(frames.size() == no_of_pages && !hit) {
            ++no_of_faults;
            int toRemove = idx++ % no_of_pages;
            frameMap.erase(frames.at(toRemove));
            frameMap.insert({page,0});
            frames[toRemove] = page;
            printf("%02d F   %s\n", page, getStringContent(frames).c_str());
        }else
        {
            if(!hit) {
                frames.push_back(page);
                frameMap.insert({page, 0});
            }
            printf("%02d     %s\n",page, getStringContent(frames).c_str());
        }
    }
    print_end(no_of_faults);
}

void lru(int no_of_pages)
{
    vector<int> stream = getStream();
    unordered_map<int,int> frameMap;
    vector<int> frames;
    bool hit = false;
    int page;
    int no_of_faults = 0;
    int time = 0;
    for(int i = 0, n = stream.size() ; i < n ; ++i)
    {
        page = stream.at(i);
        hit = frameMap.find(page) != frameMap.end();
        if(frames.size() == no_of_pages && !hit) {
            ++no_of_faults;
            int toRemove = getMostAccessed(frameMap,frames);
            frameMap.erase(frames.at(toRemove));
            frameMap.insert({page,time++});
            frames[toRemove] = page;
            printf("%02d F   %s\n", page, getStringContent(frames).c_str());
        }else
        {
            if(!hit) {
                frames.push_back(page);
                frameMap.insert({page, time++});
            }else{
                frameMap[page] = time++;
            }
            printf("%02d     %s\n",page, getStringContent(frames).c_str());
        }
    }
    print_end(no_of_faults);
}
int getMostAccessed(unordered_map<int,int> freqMap,vector<int> frames)
{
    int leastAccessed = 0;
    int minCount = INT32_MAX;
    for(pair<int,int> entry : freqMap)
    {
        if(minCount > entry.second)
        {
            minCount = entry.second;
            leastAccessed = entry.first;
        }
    }
    auto iterator = find(frames.begin(),frames.end(),leastAccessed);
    int idx = distance(frames.begin(),iterator);
    return iterator == frames.end() ? 0 : idx;
}
void clock(int no_of_pages)
{
    vector<int> stream = getStream();
    unordered_map<int,int> frameMap;
    vector<int> frames;
    vector<int> clockFrames;
    for(int i = 0 ; i < no_of_pages ; ++i)
        clockFrames.push_back(1);
    bool hit = false;
    int page;
    int no_of_faults = 0;
    int ptr = 0;
    int idx = 0;
    for(int i = 0, n = stream.size() ; i < n ; ++i)
    {
        page = stream.at(i);
        hit = frameMap.find(page) != frameMap.end();
        if(frames.size() == no_of_pages && !hit) {
            ++no_of_faults;
            ptr = getClockIdx(clockFrames,ptr);
            frameMap.erase(frames.at(ptr));
            frameMap.insert({page,ptr});
            frames[ptr] = page;
            clockFrames[ptr++] = 1;
            ptr %= no_of_pages;
            printf("%02d F   %s\n", page, getStringContent(frames).c_str());
        }else
        {
            if(!hit) {
                frames.push_back(page);
                frameMap.insert({page, idx++});
            }else{
                int accessed = frameMap[page];
                clockFrames[accessed] = 1;
            }
            printf("%02d     %s\n",page, getStringContent(frames).c_str());
        }
    }
    print_end(no_of_faults);
}

int getClockIdx(vector<int>& clockFrames, int ptr)
{
    for(;;)
    {
        if(clockFrames[ptr] == 0)
            return ptr;
        clockFrames[ptr++] = 0;
        ptr %= clockFrames.size();
    }
}


