#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

std::vector<std::string> SplitString(std::string text, char d);
std::string GetFullPath(std::string cmd/*, const std::vector<std::string>& os_path_list*/);
std::string trim(std::string toTrim);
std::vector<std::string> Split(std::string toSplit);
std::vector<std::string> SplitPath(std::string toSplit);
std::vector<std::string> SplitQuotes(std::string toSplit);
std::string Smash(std::vector<std::string> splitList);
char** StringVectorToArrayCharArray(std::vector<std::string>& vec);
void DeleteArrayCharArray(char **char_arr, int length);

int main (int argc, char **argv)
{
    //create a flag to make sure weird things do not happen, i.e. exit not working unless typed twice. Not as cool as the titular band but it does the job.
    int blackFlag;
    //create file.
    string input;
    string origInput;
    //we need to save history somewhere. Storing the history in a file and then reloading it upon running is a good idea.
    //check if a document of some arbitrary name exists, load from it if does, create the file if it doesn't.
    //we'll just call the file history.txt.
    vector<string> history;
    vector<string> temp;
    vector<string> histTemp;
    //PATH is where the file was located. Keep in mind the 'ls' hack we did in Info Sec, since this follows a similar process minus the nefarious parts
    char* os_path = getenv("PATH");
    //split on : since we're looking for the path here, not the command. I'm right about the path at least. We may be looking for the command, but I'm guessing not right now.
    vector<string> os_path_list = SplitString(os_path, ':');
    cout << "Welcome to OSShell! Please enter your commands ('exit' to quit)." << endl;
    while(1)
    {
        blackFlag = 0;
        //read in all lines from file. 
        string input;
        ifstream myfile;    
        myfile.open("history.txt");    
        string line;
        temp.clear();
        history.clear();
        if (myfile.is_open())
        {
            while (getline (myfile,line))
            {
                temp.push_back(line);
            }
        }
        myfile.close();
        //check how many lines are in the file. If there are 128 or more lines, we only read in the last 127 lines (in order to leave room for the entered command).
        //best case, the file had less than 128 lines. If not, we only read in the last 127 lines. Regardless, we dump the commands into history from temp. (By pushback)
        
        if(temp.size() < 128)
        {
           for(int k = 0; k < temp.size(); k++)
           {
               history.push_back(temp[k]);
           }
        }
        else
        {
            for(int k = temp.size()-127; k < temp.size(); k++)
            {
                history.push_back(temp[k]);
            }
        }
        
        
        //take input.
        cout << "osshell> ";
        getline(cin, input);
        //save the original input without any modifications so we can dump it into history.
        origInput = input;
        //trim the input.
        input = trim(input);
        //get a list of all elements in the input.
        vector<string> elemList = Split(input);
        //now we can smash the input back together.
        input = Smash(elemList);
        //iterate over elemList[2] if it exists and ensure that it is a number before attempting to stoi it.
        int flag = 0;
        if(elemList.size() == 2)
        {
            for(int i = 0; i < elemList[1].size(); i++)
            {
                if(elemList[1][i] != '1' && elemList[1][i] != '2' && elemList[1][i] != '3' 
                && elemList[1][i] != '4' && elemList[1][i] != '5' && elemList[1][i] != '6' 
                && elemList[1][i] != '7' && elemList[1][i] != '8' && elemList[1][i] != '9' && elemList[1][i] != '0')
                {
                    flag = 1;
                }
            }
        }
        //if there are multiple parameters in elemList and the first element is history and the second element is clear or a number
        if(elemList.size() == 2 && elemList[0] == "history" && elemList[1] != "clear" && flag == 0 && ((stoi(elemList[1]) > 0)))
        {
            blackFlag = 1;
            int start = (history.size())-(stoi(elemList[1]));
            int end;
            if(history.size() < stoi(elemList[1]))
            {
                end = stoi(elemList[1]);
            }
            else
            {
                end = history.size();
            }
            if(stoi(elemList[1]) <= history.size())
            {
                for(int i = start; i < end; i++)
                {
                    cout << "  " << i << ": " << history[i] << endl;
                }
            }
            else
            {
                for(int i = 0; i < history.size(); i++)
                {
                    cout << "  " << i << ": " << history[i] << endl;
                }
            }
        }
        //  If command is `history` print previous N commands
        if(input == "history")
        {
            for(int i = 0; i < history.size(); i++)
            {
                cout << "  " << i+1 << ": " << history[i] << endl;
            }
            blackFlag = 1;
        }
        ofstream myfile2;
        //if numberToStrip is the same as the input, there was no whitespace and this will allow history and exit to work properly.
        //this isn't a great approach, because what if they input "history ". Can we trim it, or can we just check if there is no occurence of the delimiter?
        
        if(input == "history clear")
        {
            //wipe our vector and the file.
            history.clear();
            myfile2.open("history.txt");
            myfile2.close();
            blackFlag = 1;
        }
        //deal with history and then some number as long as it's greater than 0 and less than 128. Otherwise, throw an error. 
        //history 1 will work, history       128 will work (even though it's stupid and does what history would do anyway), history 2 a would break, history 2  4 would also break.
        //push input onto history as the last element.
        if(input != "history clear")
        {
            history.push_back(origInput);
        }
        if(input == "exit")
        {
            //truncate the file before we write from history vector into our history file.
            //dump everything in our history vector into history.txt
            myfile2.open("history.txt");
            for(int j = 0; j < history.size(); j++)
            {
                line = history[j];
                myfile2 << line << endl;
            }
            myfile2.close();
            blackFlag = 1;
            break;
        }
        //if blackFlag is 0, then history or exit wasn't an input.
        if(blackFlag == 0)
        {
        /*
                ptr = strstr(my_input_string,"\"\"");
                if(ptr != 0)
                {
                   len = strlen(ptr+2);
                   // the string exists
                   memmove(ptr, ptr+1,strlen(ptr+1));
                   // now null-terminate the string
                   ptr[len] = 0;
                }
        */
            vector<string> inputVector = Split(Smash(SplitQuotes(input)));
            string fullPath = GetFullPath(inputVector[0]);
            //cout << "GETFULLPATH RETURNS: " << fullPath << endl;
            
            char** inputArray = StringVectorToArrayCharArray(inputVector);
            int status;
            pid_t pid =  fork();
            if(pid>0)
            {
                while (wait(&status) != pid);    /* wait for completion  */
            }
            else
            {
                execv(fullPath.c_str(), inputArray);
                DeleteArrayCharArray(inputArray, strlen(*inputArray));
                //dump everything in our history vector into history.txt

                myfile2.open("history.txt");

                for(int j = 0; j < history.size(); j++)
                {
                    line = history[j];
                    myfile2 << line << endl;
                }
                myfile2.close();
                exit(1);
            }
        }
        
        
        //dump everything in our history vector into history.txt
        myfile2.open("history.txt");
        for(int j = 0; j < history.size(); j++)
        {
            line = history[j];
            myfile2 << line << endl;
        }
        myfile2.close();
        

    }
    return 0;
}






// Returns vector of strings created by splitting `text` on every occurance of `d`

std::vector<std::string> SplitString(std::string text, char d)
{
    std::vector<std::string> result;

    return result;
}






// Returns a string for the full path of a command if it is found in PATH, otherwise simply return `cmd`
std::string GetFullPath(std::string cmd)
{
    //environment aka paths seperated by colons
    string env = getenv("PATH");
    //cout<<env<<endl;
    //will store path + "/" + cmd
    string path_and_cmd;
    //vector of all paths
    vector<string> paths = SplitPath(env);
    
    for(int i = 0; i<paths.size(); i++)
    {
        path_and_cmd = paths[i]+"/"+cmd;
        //cout<<"path_and_cmd: "<<path_and_cmd<<endl;
        if (FILE *file = fopen(path_and_cmd.c_str(), "r")) {
            fclose(file);
            //cout<<"FOUND COMMAND IN PATH: "<<path_and_cmd<<endl;
            return path_and_cmd;
            
        }
           
        
    }
    if(cmd.find("history")==string::npos)
    {
        cout << cmd << ": Error running command" << endl;
    }
    else
    {
        cout << "Error: history expects an integer > 0 (or 'clear')" << endl;
    }
    //cout<<"COULDN'T FIND COMMAND IN PATH"<<endl;
    return cmd;
}





//Returns a trimmed string. Just an easier way to deal with the inputs.
std::string trim(string toTrim)
{
    size_t first = toTrim.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = toTrim.find_last_not_of(' ');
    return toTrim.substr(first, (last-first+1));
}





//Returns a vector of all non-whitespace strings in our input.
std::vector<std::string> Split(std::string toSplit)
{
    string smashed;
    istringstream buffer(trim(toSplit));
    
    vector<string> splitList;
    
    copy(istream_iterator<string>(buffer), 
              istream_iterator<string>(),
              back_inserter(splitList));
    return splitList;
}

//Returns a vector of each part of our path
std::vector<std::string> SplitPath(std::string toSplit)
{
    stringstream splitMe(toSplit);
    string current;
	vector<std::string> splitList;
    while (std::getline(splitMe, current, ':'))
	{
		splitList.push_back(trim(current));
    }
	return splitList;
}
//Returns a vector of each part of our path
std::vector<std::string> SplitQuotes(std::string toSplit)
{
    stringstream splitMe(toSplit);
    string current;
	vector<std::string> splitList;
    while (std::getline(splitMe, current, '"'))
	{
		splitList.push_back(trim(current));
    }
	return splitList;
}

//returns a smashed together string
std::string Smash(std::vector<std::string> splitList)
{
    string smashed;
    //we have the vector now. Now smash everything back together with only one whitespace in between.
    for(int i = 0; i<splitList.size(); i++)
    {
        smashed = smashed + " " + splitList[i];
    }
    return trim(smashed);
}


char** StringVectorToArrayCharArray(std::vector<std::string>& vec)
{
    int i;
    char **char_arr = new char*[vec.size() + 1];
    for (i = 0; i < vec.size(); i++)
    {
        char_arr[i] = new char[vec[i].length() + 1];
        strcpy(char_arr[i], vec[i].c_str());
    }
    char_arr[vec.size()] = NULL;

    return char_arr;
}

void DeleteArrayCharArray(char **char_arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        delete[] char_arr[i];
    }
    delete[] char_arr;
}


