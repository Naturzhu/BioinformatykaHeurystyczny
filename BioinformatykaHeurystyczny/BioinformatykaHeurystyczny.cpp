#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

class Node1 {
public:
    string olig;
    long unsigned int index;
    int matches;
};

vector<Node1> nodes{};

vector<int> oligsToAppend(int indexToStart)
{
    vector<int> resultVector{};
    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = 0; j < nodes[i].matches; j++)
        {
            resultVector.push_back(i);
        }
        if (i == indexToStart)
        {
            resultVector.erase(resultVector.begin() + resultVector.size() - 1);
        }
    }
    return resultVector;
}

class Unit {
public:
    string result;
    int fit;
    vector<int> usedNodes{};

    void fitCalculator(int desiredLength, int probe)
    {
        fit = 0;
        for (int i = 1; i < usedNodes.size(); i++)
        {
            for (int j = 0; j < probe - 1; j++)
            {
                if (nodes[usedNodes[i]].olig[j] == nodes[usedNodes[i - 1]].olig[probe - 1 - j])
                {
                    fit++;
                }
                else
                {
                    break;
                }
            }
        }
        if (desiredLength > result.size())
        {
            fit -= desiredLength - result.size();
        }
        else
        {
            fit -= result.size() - desiredLength;
        }
    }
    
    void resultCalculator(int probe)
    {
        result.clear();
        result += nodes[usedNodes[0]].olig;
        for (int i = 1; i < usedNodes.size(); i++)
        {
            for (int j = 0; j < probe - 1; j++)
            {
                if (nodes[usedNodes[i]].olig[j] != nodes[usedNodes[i - 1]].olig[probe - 1 - j])
                {
                    result += nodes[usedNodes[i]].olig.substr(j);
                    break;
                }
            }
        }
    }

    void randomizeNodes(int indexToStart)
    {
        vector<int> toAdd = oligsToAppend(indexToStart);
        usedNodes.push_back(nodes[indexToStart].index);
        int next;
        while (toAdd.size() > 0)
        {
            next = rand() % toAdd.size();
            usedNodes.push_back(toAdd[next]);
            toAdd.erase(toAdd.begin() + next);
        }
    }

    void crossNodes(Unit parent1, Unit parent2, int length, int fragmentSize)
    {
        usedNodes.clear();
        int fragmentsNumber = length / fragmentSize;
        if (length % fragmentSize)
        {
            fragmentsNumber++;
        }
        for (int i = 0, parent; i < fragmentsNumber; i++)
        {
            parent = rand() % 2;
            if (parent)
            {
                for (int j = i * fragmentSize; j < (i + 1) * fragmentSize && j < parent2.usedNodes.size(); j++)
                {
                    usedNodes.push_back(parent2.usedNodes[j]);
                }
            }
            else
            {
                for (int j = i * fragmentSize; j < (i + 1) * fragmentSize && j < parent1.usedNodes.size(); j++)
                {
                    usedNodes.push_back(parent1.usedNodes[j]);
                }
            }
        }
        
        vector<Node1> nodesCopy = nodes;
        vector<int> indexesToFix{};
        vector<int> indexesToAdd{};
        
        for (int i = 0; i < usedNodes.size(); i++)
        {
            if (nodesCopy[usedNodes[i]].matches > 0)
            {
                nodesCopy[usedNodes[i]].matches--;
            }
            else
            {
                indexesToFix.push_back(i);
            }
        }

        for (int i = 0; i < nodesCopy.size(); i++)
        {
            for (int j = 0; j < nodesCopy[i].matches; j++)
            {
                indexesToAdd.push_back(i);
            }
        }
        if (indexesToAdd.size() == indexesToFix.size())
        {
            for (int i = 0; i < indexesToFix.size(); i++)
            {
                usedNodes[indexesToFix[i]] = indexesToAdd[i];
            }
        }
    }
};

int randomizeMatch(int lower, int upper)
{
    return (lower + (rand() % (upper - lower + 1)));
}

int intensityToMatch(int intensity)
{
    switch (intensity)
    {
    case 0:
        return 0;
        break;
    case 1:
        return 1;
        break;
    case 2:
        return 1;
        break;
    case 3:
        //return randomizeMatch(1,2);
        return 1;
        break;
    case 4:
        return 2;
        break;
    case 5:
        //return randomizeMatch(2,3);
        return 2;
        break;
    case 6:
        //return randomizeMatch(3,4);
        return 3;
        break;
    case 7:
        //return randomizeMatch(3, 6);
        return 3;
        break;
    case 8:
        //return randomizeMatch(4, 8);
        return 4;
        break;
    case 9:
        //return randomizeMatch(6, 9);
        return 6;
        break;
    default:
        return 0;
    }
}

string finalResult{};
int totalNodes;

int genetic(int indexToStart, int probe, int length, int bestInGenerationSizePercent, int numberOfTries, int fragmentSize, int mutationsInGenerationPercent)
{
    int generationSize = 1000;
    int mutationsInUnit = length/50 + 1;
    int bestInGenerationSize = (generationSize * bestInGenerationSizePercent) / 100;
    vector<Unit> generation{};
    vector<int>bestFit(bestInGenerationSize, 0);
    vector<int>bestIndex(bestInGenerationSize, 0);
    vector<int>bestInLastGenerations(numberOfTries, 0);
    int mutationsInGeneration = (generationSize * mutationsInGenerationPercent) / 100;
    for (int i = 0; i < generationSize; i++)
    {
        Unit one{};
        one.randomizeNodes(indexToStart);
        one.resultCalculator(probe);
        one.fitCalculator(length, probe);
        generation.push_back(one);
        if (i < bestInGenerationSize)
        {
            if (i == 0)
            {
                bestFit[i] = one.fit;
                bestIndex[i] = i;
            }
            else
            {
                for (int j = 0; j < i; j++)
                {
                    if (one.fit > bestFit[j])
                    {
                        for (int k = i; k >= j; k--)
                        {
                            if (k < bestInGenerationSize - 1)
                            {
                                bestFit[k + 1] = bestFit[k];
                                bestIndex[k + 1] = bestIndex[k];
                            }
                        }
                        bestFit[j] = one.fit;
                        bestIndex[j] = i;
                        break;
                    }
                }
            }
        }
        else
        {
            for (int j = 0; j < bestInGenerationSize; j++)
            {
                if (one.fit > bestFit[j])
                {
                    for (int k = bestInGenerationSize - 2; k >= j; k--)
                    {
                        bestFit[k + 1] = bestFit[k];
                        bestIndex[k + 1] = bestIndex[k];
                    }
                    bestFit[j] = one.fit;
                    bestIndex[j] = i;
                    break;
                }
            }
        }
    }
    int actualMutations;
    for (int generationNumber = 0; generationNumber < numberOfTries; generationNumber++)
    {
        actualMutations = 0;
        for (int i = 0; i < generationSize; i++)
        {
            bool oneOfTheBest = std::find(std::begin(bestIndex), std::end(bestIndex), i) != std::end(bestIndex);
            if (!oneOfTheBest)
            {
                if (actualMutations >= mutationsInGeneration)
                {
                    int parent1 = rand() % bestInGenerationSize;
                    int parent2 = rand() % bestInGenerationSize;
                    while (parent2 == parent1)
                    {
                        parent2 = rand() % bestInGenerationSize;
                    }
                    generation[i].crossNodes(generation[bestIndex[parent1]], generation[bestIndex[parent2]], length, fragmentSize);
                    generation[i].resultCalculator(probe);
                    generation[i].fitCalculator(length, probe);
                }
                else
                {
                    int parent1 = rand() % bestInGenerationSize;
                    int parent2 = rand() % bestInGenerationSize;
                    while (parent2 == parent1)
                    {
                        parent2 = rand() % bestInGenerationSize;
                    }
                    generation[i].crossNodes(generation[bestIndex[parent1]], generation[bestIndex[parent2]], length, fragmentSize);
                    int index1, index2, temp;
                    for (int j = 0; j < mutationsInUnit; j++)
                    {
                        index1 = rand() % generation[i].usedNodes.size();
                        index2 = rand() % generation[i].usedNodes.size();
                        temp = generation[i].usedNodes[index2];
                        generation[i].usedNodes[index2] = generation[i].usedNodes[index1];
                        generation[i].usedNodes[index1] = temp;
                    }
                    generation[i].resultCalculator(probe);
                    generation[i].fitCalculator(length, probe);
                    actualMutations++;
                }
            }
        }
        for (int i = 0; i < generationSize; i++)
        {
            bool oneOfTheBest = std::find(std::begin(bestIndex), std::end(bestIndex), i) != std::end(bestIndex);
            if (!oneOfTheBest)
            {
                for (int j = 0; j < bestInGenerationSize; j++)
                {
                    if (generation[i].fit > bestFit[j])
                    {
                        for (int k = bestInGenerationSize - 2; k >= j; k--)
                        {
                            bestFit[k + 1] = bestFit[k];
                            bestIndex[k + 1] = bestIndex[k];
                        }
                        bestFit[j] = generation[i].fit;
                        bestIndex[j] = i;
                        break;
                    }
                }
            }
        }
        bestInLastGenerations[numberOfTries - generationNumber - 1] = bestFit[0];
    }
    while (bestInLastGenerations[0] != bestInLastGenerations[numberOfTries - 1])
    {
        actualMutations = 0;
        for (int i = 0; i < generationSize; i++)
        {
            bool oneOfTheBest = std::find(std::begin(bestIndex), std::end(bestIndex), i) != std::end(bestIndex);
            if (!oneOfTheBest)
            {
                if (actualMutations >= mutationsInGeneration)
                {
                    int parent1 = rand() % bestInGenerationSize;
                    int parent2 = rand() % bestInGenerationSize;
                    while (parent2 == parent1)
                    {
                        parent2 = rand() % bestInGenerationSize;
                    }
                    generation[i].crossNodes(generation[bestIndex[parent1]], generation[bestIndex[parent2]], length, fragmentSize);
                    generation[i].resultCalculator(probe);
                    generation[i].fitCalculator(length, probe);
                }
                else
                {
                    generation[i].randomizeNodes(indexToStart);
                    generation[i].resultCalculator(probe);
                    generation[i].fitCalculator(length, probe);
                    actualMutations++;
                }
            }
        }
        for (int i = 0; i < generationSize; i++)
        {
            bool oneOfTheBest = std::find(std::begin(bestIndex), std::end(bestIndex), i) != std::end(bestIndex);
            if (!oneOfTheBest)
            {
                for (int j = 0; j < bestInGenerationSize; j++)
                {
                    if (generation[i].fit > bestFit[j])
                    {
                        for (int k = bestInGenerationSize - 2; k >= j; k--)
                        {
                            bestFit[k + 1] = bestFit[k];
                            bestIndex[k + 1] = bestIndex[k];
                        }
                        bestFit[j] = generation[i].fit;
                        bestIndex[j] = i;
                        break;
                    }
                }
            }
        }
        for (int i = numberOfTries - 2; i >= 0; i--)
        {
            bestInLastGenerations[i + 1] = bestInLastGenerations[i];
        }
        bestInLastGenerations[0] = bestFit[0];
    }
    return generation[bestIndex[0]].fit;
}

int main()
{
    srand(time(NULL));
    ifstream inputFile;
    inputFile.open("input.xml");
    string linia;
    getline(inputFile, linia);
    int indeks1;
    indeks1 = linia.find("length=\"");
    int indeks2;
    indeks2 = linia.find("\" start=\"");
    int indeks3;
    indeks3 = linia.find("\">");
    int length = stoi(linia.substr((indeks1+8),indeks2- (indeks1 + 8)));
    string start = linia.substr((indeks2 + 9), indeks3 - (indeks2 + 9));

    for (int i = 0; i < 15; i++)
    {
        getline(inputFile, linia);
    }
    
    int probe = start.size();

    int indexToStart = 0;

    getline(inputFile, linia);
    int i = 0;
    int intensity;
    string oligonucleotide;
    totalNodes = 0;

    while (linia.find("</probe>") == string::npos)
    {
        //dla łatwego inputu:
        /*
        indeks2 = linia.find("l>");
        indeks3 = linia.find("</cell>");
        oligonucleotide = linia.substr((indeks2 + 2), indeks3 - (indeks2 + 2));
        intensity = 1;
        */
        //dla zwyklego inputu:
        
        indeks1 = linia.find("ity=\"");
        indeks2 = linia.find("\">");
        intensity = stoi(linia.substr((indeks1 + 5), 1));
        oligonucleotide = linia.substr((indeks2 + 2), probe);
        
        Node1 x{};
        x.index = i;
        x.olig = oligonucleotide;
        x.matches = intensityToMatch(intensity);
        totalNodes += intensityToMatch(intensity);
        nodes.push_back(x);
        if (start == oligonucleotide) {
            indexToStart = i;
        }
        getline(inputFile, linia);
        i++;
    } 
    cout << "Length: " << length << " Probe: " << probe << endl;
    int bestInGenerationSizePercent = 40;
    int timesUntilStop = 100;
    int fragmentSize = 5;
    int mutationChance = 10;
    int fit;
    auto startTime = chrono::steady_clock::now();
    fit = genetic(indexToStart, probe, length, bestInGenerationSizePercent, timesUntilStop,fragmentSize,  mutationChance);
    auto endTime = chrono::steady_clock::now();
    cout << "Dla parametrow: " << bestInGenerationSizePercent << " " << timesUntilStop << " " << fragmentSize << " " << mutationChance << " uzyskano wynik: " << fit << endl;
    cout << "Trwalo to: " << chrono::duration_cast<chrono::seconds>(endTime - startTime).count() << " sekund." << endl;
}
