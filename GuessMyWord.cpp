#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>

class giveMeDate {

private:
    struct tm newtime;
    __time32_t aclock;
    char buffer[32];
    errno_t errNum;

public:
    giveMeDate();
};

giveMeDate::giveMeDate()
{
    _time32(&aclock);
    _localtime32_s(&newtime, &aclock);

    errNum = asctime_s(buffer, 32, &newtime);
    std::cout << "Welcome : " << buffer << std::endl;
}

class guessMyWord : giveMeDate {

public:
    guessMyWord(std::string& myBook, int length = 4);
    bool bAnagram(std::string lhs, std::string rhs);
    void showSecretWords(std::string& guess, bool hiddenWord);
    // all words from the dictionary
    std::vector<std::string> words;
    std::pair<std::string, std::string> secretWord; // !!!

private:
    size_t randomNumber(const int min_, const int max_);
    void selectWord();
    std::string shuffleWord(std::string& word);
};
// class constructor
guessMyWord::guessMyWord(std::string& myBook, int length)
{
    std::string word; // single buffer
    std::ifstream input_file(myBook);

    if (!input_file.is_open())
    {
        std::cout << "Could not open the file " << myBook << std::endl;
        exit(EXIT_FAILURE);
    }

    while (input_file >> word)
        if (word.size() == length)
            words.push_back(word);

    input_file.close();
    // selects one word in the main vector
    selectWord();
}

size_t guessMyWord::randomNumber(const int min_, const int max_)
{
    std::random_device rd;
    std::mt19937 rng{ rd() };
    std::uniform_int_distribution<int> uid(min_, max_);

    return uid(rng);
}

void guessMyWord::selectWord()
{
    int rn = randomNumber(1, words.size() - 1);
    secretWord.first = words[rn];
    secretWord.second = shuffleWord(words[rn]); // debug mode
//  std::cout << "Hidden word : " << secretWord.first << " " << secretWord.second << std::endl;
}

std::string guessMyWord::shuffleWord(std::string& word) {
    
    std::string tmp{ word };

    do {
        std::shuffle(tmp.begin(), tmp.end(), std::default_random_engine(unsigned int(randomNumber(1, tmp.size() - 1))));
    } while (std::find(words.begin(), words.end(), tmp) != words.end());

    return tmp;
}

bool guessMyWord::bAnagram(std::string lhs, std::string rhs) 
{
    std::sort(lhs.begin(), lhs.end());
    std::sort(rhs.begin(), rhs.end());

    return lhs == rhs;
}

void guessMyWord::showSecretWords(std::string& guess, bool showHiddenWord)
{
    std::cout << std::endl;
    std::cout << "Anagrams available for this word (alternative good answers) :" << std::endl;
    int occ = 0;

    if (showHiddenWord)
    {
        std::cout << secretWord.first << std::endl;
        ++occ;
    }

    for (size_t i = 0; i < words.size(); ++i)
    {
        if (bAnagram(secretWord.first, words[i]) && guess != words[i])
        {
            std::cout << words[i] << std::endl;
            ++occ;
        }
    }
    // no occurrence for anagrams
    occ == 0 ? std::cout << "[nothing]" << std::endl << std::endl
             : std::cout << std::endl;
}

int main()
{
    std::string dico = ".\\dictionary.txt";
    int letters = 4;

    while (true)
    {
        int tries{};
        std::string myGuess;
        // load dico, select a word, shuffle it...
        guessMyWord *gmw = new guessMyWord(dico, letters);

        std::cout << "~~ Guess My Word ~~" << std::endl;
        std::cout << "Words in the dictionary with " << letters << " letters : " << gmw->words.size() << std::endl;
        std::cout << std::endl;

        do
        {
            std::cout << "What is your suggestion for the letters [" << gmw->secretWord.second << "] ? " << std::endl;
            std::cin >> myGuess;
            // to lower case
            std::transform(myGuess.begin(), myGuess.end(), myGuess.begin(), [](unsigned char c) { return (unsigned char)std::tolower(c); });
            ++tries;

            if (myGuess == "*cheat")
            {
                gmw->showSecretWords(gmw->secretWord.first, true);
                system("pause");
                return EXIT_SUCCESS;
            }
            // boring game?
            if (myGuess == "exit")
                return EXIT_SUCCESS;
            // not the right word length
            if (myGuess.length() != gmw->secretWord.first.length())
                std::cout << "Not the same length" << std::endl;
            else if (!gmw->bAnagram(myGuess, gmw->secretWord.first)) // one or more letters are not in the secret word
                std::cout << "Not the right letters" << std::endl; // the letters are good, the length too, but this word is weird
            else if (gmw->bAnagram(myGuess, gmw->secretWord.first) && std::find(gmw->words.begin(), gmw->words.end(), myGuess) == gmw->words.end())
                std::cout << "It seems that this word does not exist" << std::endl;
        } while (!gmw->bAnagram(myGuess, gmw->secretWord.first) || std::find(gmw->words.begin(), gmw->words.end(), myGuess) == gmw->words.end());
        // check if two strings are anagrams and if the guessed word exists in the dictionary - or loop again
        std::cout << "Correct! You got it in " << tries << " guesses!" << std::endl;
        // display alternatives - except the guessed word
        gmw->showSecretWords(myGuess, false);
        // increase difficulty
        ++letters;
        // max difficulty
        if (letters > 14)letters = 14;
        // clean up
        delete gmw;
    }

    return EXIT_SUCCESS;
}
