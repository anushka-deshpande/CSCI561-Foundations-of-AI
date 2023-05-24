#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

int maxDepth = 1;
char ourPlayer;
char opponentPlayer;
vector<vector<char> > board;
vector<char>temp;
int beta;

string ourplayer;
double secondsLeft;
string allCaptured;
int whiteCaptured;
int blackCaptured;
char boardLoc;
int xPos;
char yPos;

string whiteFive = "wwwww";
string whiteFour = ".wwww";
string whiteThreeArray[] = {".w.ww", "..www", ".www.", ".ww.w"};
string whiteCapSequence = ".wwb";


string blackFive = "bbbbb";
string blackFour = ".bbbb";
string blackThreeArray[] = {"..bbb", ".b.bb", ".bb.b", ".bbb."};
string blackCapSequence = ".bbw";


void readFile()
{
    ifstream inFile;
    inFile.open("input.txt");

    if (!inFile) 
    {
        cerr << "Unable to open file input.txt";
        exit(1);
    }

    inFile >> ourplayer >> secondsLeft >> allCaptured;

    if(secondsLeft<10)
    {
        maxDepth = 0;
    }

    int pos = allCaptured.find(",");

    whiteCaptured = stoi(allCaptured.substr(0, pos));
    blackCaptured = stoi(allCaptured.substr(pos + 1));

    for (int i = 0; i < 19; i++) 
    {
        temp.clear();
        for (int j = 0; j < 19; j++) 
        {
            inFile >> boardLoc;
            temp.push_back(boardLoc);
        }
        board.push_back(temp);
    }
    inFile.close();
}

void writeFile()
{
    ofstream outFile;
    outFile.open("output.txt");

    if (!outFile) 
    {
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }

    outFile << xPos << yPos;
    outFile.close();

}


void findApplyCaptures(vector<vector<char> > board, int i, int j, char player) 
{
    char opponentColour = player=='w'? 'b':'w';

    if(i-3 >= 0)
    {
        if(board[i-3][j] == player && board[i-2][j] == opponentColour && board[i-2][j] == opponentColour)
        {
            board[i-1][j] = '.';
            board[i-2][j] = '.';
        }
    }
    
    if(i+3 < 19)
    {
        if(board[i+3][j] == player && board[i+2][j] == opponentColour && board[i+1][j] == opponentColour)
        {
            board[i+2][j] = '.';
            board[i+2][j] = '.';
        }
    }

    if(j-3 >= 0)
    {
        if(board[i][j-3] == player && board[i][j-2] == opponentColour && board[i][j-1] == opponentColour)
        {
            board[i][j-1] = '.';
            board[i][j-2] = '.';
        }
    }

    if(j+3 < 19)
    {
        if(board[i][j+3] == player && board[i][j+2] == opponentColour && board[i][j+1] == opponentColour)
        {
            board[i][j+1] = '.';
            board[i][j+2] = '.';
        }
    }


    if(i-3 >= 0 && j-3 >= 0)
    {
        if(board[i-3][j-3] == player && board[i-2][j-2] == opponentColour && board[i-2][j-1] == opponentColour)
        {
            board[i-1][j-1] = '.';
            board[i-2][j-2] = '.';
        }
    }

    if(i+3 < 19 && j+3 < 19)
    {
        if(board[i+3][j+3] == player && board[i+2][j+2] == opponentColour && board[i+1][j+1] == opponentColour)
        {
            board[i+2][j+1] = '.';
            board[i+2][j+2] = '.';
        }
    }

    if(i-3 >= 0 && j+3 < 19)
    {
        if(board[i-3][j+3] == player && board[i-2][j+2] == opponentColour && board[i-2][j+1] == opponentColour)
        {
            board[i-1][j+1] = '.';
            board[i-2][j+2] = '.';
        }
    }

    if(i+3 < 19 && j-3 >= 0)
    {
        if(board[i+3][j-3] == player && board[i+2][j-2] == opponentColour && board[i+1][j-1] == opponentColour)
        {
            board[i+2][j-2] = '.';
            board[i+2][j-1] = '.';
        }
    }
}

class Node 
{
    public:
        vector<vector<char> > board;
        int row;
        int col;
        char player;
        int depth;
        vector<Node> children;

        Node(vector<vector<char> > board, int row, int col, char player, int depth) 
        {
            this->board = board;
            this->row = row;
            this->col = col;
            this->player = player;
            this->depth = depth;
        }

        void generateChildren() 
        {
            int top = 19;
            int left = 19;
            int bottom = -1;
            int right = -1;
            for(int i=0;i<19;i++)
            {
                for(int j=0;j<19;j++)
                {
                    if(board[i][j] != '.')
                    {
                        if(i<top)
                        {
                            top = i;
                        }
                        if(j<left)
                        {
                            left = j;
                        }
                        if(i > bottom)
                        {
                            bottom = i;
                        }
                        if(j > right)
                        {
                            right = j;
                        }
                    }
                }
            }

            int i=0;

            while(i<1)
            {
                if(top != 0) top--;
                if(left != 0) left--;
                if(bottom != 18) bottom++;
                if(right != 18) right++;
                i++;
            }

            for (i = top; i <= bottom; i++) 
            {
                for (int j = left; j <= right; j++) 
                {
                    if (board[i][j] == '.') 
                    {
                        vector<vector<char> > newBoard = board;
                        newBoard[i][j] = player;
                        findApplyCaptures(newBoard, i, j, player);
                        Node child_node(newBoard, i, j, player == 'b' ? 'w' : 'b', depth + 1);
                        children.push_back(child_node);
                    }
                }
            }
        }

        bool isWin(char player) 
        {
            
            int count = 0;

            for (int i = 0; i < 19; i++) 
            {
                count = 0;
                for (int j = 0; j < 19; j++) 
                {
                    if (board[i][j] == player) 
                    {
                        count++;
                    } 
                    else 
                    {
                        count = 0;
                    }
                    if (count == 5) 
                    {
                        return true;
                    }
                }

                count = 0;

                for (int j = 0; j < 19; j++) 
                {
                    if (board[j][i] == player) 
                    {
                        count++;
                    } 
                    else 
                    {
                        count = 0;
                    }
                    if (count == 5)
                    {
                        return true;
                    }
                }
            }

            for (int i = 0; i <= 14; i++) 
            {
                for (int j = 0; j <= 14; j++) 
                {
                    int count = 0;
                    for (int k = 0; k < 5; k++) 
                    {
                        if (board[i + k][j + k] == player) 
                        {
                            count++;
                        } 
                        else 
                        {
                            count = 0;
                        }
                        if (count == 5) 
                        {
                            return true;
                        }
                    }
                }
            }

            for (int i = 4; i < 19; i++) 
            {
                for (int j = 0; j <= 14; j++) 
                {
                    int count = 0;
                    for (int k = 0; k < 5; k++) 
                    {
                        if (board[i - k][j + k] == player) 
                        {
                            count++;
                        } 
                        else 
                        {
                            count = 0;
                        }
                        if (count == 5) 
                        {
                            return true;
                        }
                    }
                }
            }

            string capturePattern = "";
            string rowPattern = "";
            int capturedPieces = 0;
            char otherPlayer = player == 'w' ? 'b' : 'w';

            if(player == 'w')
            {
                capturedPieces = whiteCaptured;
            }
            else
            {
                capturedPieces = blackCaptured;
            }

            capturePattern += player + otherPlayer + otherPlayer + player;

            if(capturedPieces==4)
            {
                for(int i=0;i<19;i++)
                {
                    rowPattern = "";
                    for(int j=0;j<19;j++)
                    {
                        rowPattern += board[i][j];
                    }
                    if(rowPattern.find(capturePattern) != string::npos)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        int calculateScore(string str, int value, int i, int j)
        {

            int count = 0;
            int score = 0;
            int l = 0;
            int diag = 0;

            
            count = 0;
            l=0;
            for(int k=j;k<5+j;k++)
            {
                if(k<19)
                {
                    if(board[i][k] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            for(int k=i;k<5+i;k++)
            {
                if(k<19)
                {
                    if(board[k][j] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            for(int k=j;k>=j-4;k--)
            {
                if(k>=0)
                {
                    if(board[i][k] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            for(int k=i;k>=i-4;k--)
            {
                if(k>=0)
                {
                    if(board[k][j] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            diag = 0;
            while(diag!=5)
            {
                if(i-diag>=0 && j-diag>=0)
                {
                    if(board[i-diag][j-diag] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
                diag++;
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            diag = 0;
            while(diag!=5)
            {
                if(i+diag<19 && j-diag>=0)
                {
                    if(board[i+diag][j-diag] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
                diag++;
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            l=0;
            diag = 0;
            while(diag!=5)
            {
                if(i-diag>=0 && j+diag<19)
                {
                    if(board[i-diag][j+diag] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
                diag++;
            }

            if(count == 5)
            {
                score += value;
            }  

            count = 0;
            l=0;
            diag = 0;
            while(diag!=5)
            {
                if(i+diag<19 && j+diag<19)
                {
                    if(board[i+diag][j+diag] == str[l])
                    {
                        count++;
                        l++;
                    }
                }
                diag++;
            }

            if(count == 5)
            {
                score += value;
            }            
            return score;      
        }

        bool possibleCapture(int i, int j, char player, int i_offset, int j_offset) 
        {
            int capture_count = 0;
            char opponent_player = (player == 'b') ? 'w' : 'b';

            int three_i_offset = i_offset*3;
            int three_j_offset = j_offset*3;
            if(i+three_i_offset>=0 && i+three_i_offset<19 && j+three_j_offset>=0 && j+three_j_offset<19)
            {
                if(board[i][j] == player && board[i+i_offset][j+j_offset] == opponent_player && board[i+i_offset+i_offset][j+j_offset+j_offset] == opponent_player
                && board[i+three_i_offset][j+three_j_offset]==player)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            return false;
        }

        int capturesCount(int i, int j, char player) 
        {

            int captures = 0;

            captures += possibleCapture(i, j, player, -1, -1); 
            captures += possibleCapture(i, j, player, -1, 0);
            captures += possibleCapture(i, j, player, -1, 1); 
            captures += possibleCapture(i, j, player, 0, -1);
            captures += possibleCapture(i, j, player, 0, 1);
            captures += possibleCapture(i, j, player, 1, -1); 
            captures += possibleCapture(i, j, player, 1, 0); 
            captures += possibleCapture(i, j, player, 1, 1);

            return captures;
        }

        int solveFourPattern(int value, int i, int j, char player)
        {
            int count = 0;
            int score = 0;
            int diag = 0;

            if(j-1>=0 && j+3<19)
            {
                for(int c = j-1; c<=j+3; c++)
                {
                    if(board[i][c]== player)
                    {
                        count++;
                    }
                }
            }
            if(count == 5)
            {
                score += value;
            }   

            count = 0;
            if(i-1>=0 && i+3<19)
            {
                for(int r = i-1; r<=i+3; r++)
                {
                    if(board[r][j]== player)
                    {
                        count++;
                    }
                }
            }
            if(count == 5)
            {
                score += value;
            }  

            count = 0;
            if(i+1<19 && i-3>=0)
            {
                for(int r = i-3; r<=i+1; r++)
                {
                    if(board[r][j]== player)
                    {
                        count++;
                    }
                }
            }
            if(count == 5)
            {
                score += value;
            }  

            count = 0;
            diag = -1;
            if(i-1>=0 && j-1>=0 && i+3<19 && j+3<19)
            {
                while(diag!=4)
                {
                    if(board[i+diag][j+diag]==player)
                    {
                        count++;
                    }
                    diag++;
                }
            }
            if(count == 5)
            {
                score += value;
            }  


            count = 0;
            diag = -1;
            if(i+1<19 && i-3>=0 && j-1>=0 && j+3<19)
            {
                while(diag!=4)
                {
                    if(board[i-diag][j+diag] == player)
                    {
                        count++;
                    }
                    diag++;
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            if(j-3>=0 && j+1<19)
            {
                for(int c = j-3; c<=j+1; c++)
                {
                    if(board[i][c]== player)
                    {
                        count++;
                    }
                }
            }
            if(count == 5)
            {
                score += value;
            }

            count = 0;
            diag = -3;
            if(i-3>=0 && j-3>=0 && i+1<19 && j+1<19)
            {
                while(diag!=2)
                {
                    if(board[i+diag][j+diag]==player)
                    {
                        count++;
                    }
                    diag++;
                }
            }
            if(count == 5)
            {
                score += value;
            }  

            count = 0;
            diag = -3;
            if(i+3<19 && i-1>=0 && j-3>=0 && j+1<19)
            {
                while(diag!=2)
                {
                    if(board[i-diag][j+diag] == player)
                    {
                        count++;
                    }
                    diag++;
                }
            }

            if(count == 5)
            {
                score += value;
            }

            count = 0;
            if(j-2>=0 && j+2<19)
            {
                for(int r=j-2; r<=j+2;r++)
                {
                    if(board[i][r]==player)
                    {
                        count++;
                    }
                }
            }
            if(count==5)
            {
                score += value;
            }

            count = 0;
            if(i-2>=0 && i+2<19)
            {
                for(int r=i-2; r<=i+2;r++)
                {
                    if(board[r][j]==player)
                    {
                        count++;
                    }
                }
            }
            if(count==5)
            {
                score += value;
            }        

            count = 0;
            diag = -2;
            if(i-2>=0 && i+2<19 && j-2>=0 && j+2<19)
            {
                while(diag!=3)
                {
                    if(board[i+diag][j+diag] == player)
                    {
                        count++;
                    }
                    diag++;
                }
            }
            if(count==5)
            {
                score += value;
            }               

            count = 0;
            diag = -2;
            if(i-2>=0 && i+2<19 && j-2>=0 && j+2<19)
            {
                while(diag!=3)
                {
                    if(board[i-diag][j+diag] == player)
                    {
                        count++;
                    }
                    diag++;
                }
            }
            if(count==5)
            {
                score += value;
            }  
            return score;
        }

        int solveThreePattern(int value, int i, int j, char player)
        {
            int count = 0;
            int score = 0;

            if(j-3>=0)
            {
                if(board[i][j-3]==player && board[i][j-2]==player && board[i][j-1]==player)
                {
                    score += value;
                }
            }

            if(j+3<19)
            {
                if(board[i][j+3]==player && board[i][j+2]==player && board[i][j+1]==player)
                {
                    score += value;
                }
            }

            if(i-3>=0)
            {
                if(board[i-3][j]==player && board[i-2][j]==player && board[i-1][j]==player)
                {
                    score += value;
                }
            }

            if(i+3<19)
            {
                if(board[i+3][j]==player && board[i+2][j]==player && board[i+1][j]==player)
                {
                    score += value;
                }
            }

            if(i-3>=0 && j-3>=0)
            {
                if(board[i-3][j-3]==player && board[i-2][j-2]==player && board[i-1][j-1]==player)
                {
                    score += value;
                }
            }

            if(i+3<19 && j-3>=0)
            {
                if(board[i+3][j-3]==player && board[i+2][j-2]==player && board[i+1][j-1]==player)
                {
                    score += value;
                }
            }

            if(i-3>=0 && j+3<19)
            {
                if(board[i-3][j+3]==player && board[i-2][j+2]==player && board[i-1][j+1]==player)
                {
                    score += value;
                }
            }

            if(i+3<19 && j+3<19)
            {
                if(board[i+3][j+3]==player && board[i+2][j+2]==player && board[i+1][j+1]==player)
                {
                    score += value;
                }
            }

            if(i-1>=0 && i+2<19)
            {
                if(board[i-1][j]==player && board[i+1][j]==player && board[i+2][j]==player)
                {
                    score += value;
                }
            }

            if(i-1>=0 && j-1>=0 && i+2<19 && j+2<19)
            {
                if(board[i-1][j-1]==player && board[i+1][j+1]==player && board[i+2][j+2]==player)
                {
                    score += value;
                }
            }

            if(j-1>=0 && j+2<19)
            {
                if(board[i][j-1]==player && board[i][j+1]==player && board[i][j+2]==player)
                {
                    score += value;
                }
            }

            if(i+1<19 && i-2>=0 && j-1>=0 && j+2<19)
            {
                if(board[i+1][j-1]==player && board[i-1][j+1]==player && board[i-2][j+2]==player)
                {
                    score += value;
                }
            }

            if(i-2>=0 && i+1<19)
            {
                if(board[i-2][j]==player && board[i-1][j]==player && board[i+1][j]==player)
                {
                    score += value;
                }
            }

            if(i-2>=0 && j-2>=0 && i+1<19 && j+1<19)
            {
                if(board[i-2][j-2]==player && board[i-1][j-1]==player && board[i+1][j+1]==player)
                {
                    score += value;
                }
            }

            if(j-2>=0 && j+1<19)
            {
                if(board[i][j-2]==player && board[i][j-1]==player && board[i][j+1]==player)
                {
                    score += value;
                }
            }

            if(i+2<19 && i-1>=0 && j-2>=0 && j+1<19)
            {
                if(board[i+2][j-2]==player && board[i+1][j-1]==player && board[i-1][j+1]==player)
                {
                    score += value;
                }
            }

            return score;
        }

        int solveSingle(int value, int i, int j, char player)
        {

            int score = 0;

            if(i-1>=0 && j-1>=0 && board[i-1][j-1]==player)
            {
                score+=value;
            }

            if(j-1>=0 && board[i][j-1]==player)
            {
                score+=value;
            }

            if(i+1<19 && j-1>=0 && board[i+1][j-1]==player)
            {
                score+=value;
            }

            if(i+1<19 && board[i+1][j]==player)
            {
                score+=value;
            }

            if(i+1<19 && j+1<19 && board[i+1][j+1]==player)
            {
                score+=value;
            }

            if(j+1<19 && board[i][j+1]==player)
            {
                score+=value;
            }

            if(i-1>=0 && j+1<19 && board[i-1][j+1]==player)
            {
                score+=value;
            }

            if(i-1>=0 && board[i-1][j]==player)
            {
                score+=value;
            }

            return score;
            
        }

        int evaluate(char player)
        {

            int score = 0;

            for(int i=0;i<19;i++)
            {

                for(int j=0;j<19;j++)
                {

                    if(player=='w')
                    {

                        score+=calculateScore(whiteFive, 1000000, i, j);

                        score+=-calculateScore(blackFive, 1000000, i, j);

                        score+=calculateScore(whiteFour, 100000, i, j);
                        
                        score+=solveFourPattern(100000, i, j, 'w');

                        int countCaptured = capturesCount(i,j, player);
                        if(countCaptured > 0)
                        {
                            score+=100000;
                        }

                        score+=-calculateScore(blackFour, 50000, i, j);
                        
                        score+=-solveFourPattern(50000, i, j, 'b');

                        if(blackCaptured > whiteCaptured)
                        {
                            score+=calculateScore(whiteCapSequence, 500, i, j);
                        }

                        for(auto str: blackThreeArray)
                        {
                            score+=-calculateScore(str, 500, i, j);
                        }
                        
                        score+=-solveThreePattern(500, i, j, 'b');

                        score+= countCaptured*100;

                        for(auto str: whiteThreeArray)
                        {
                            score+=calculateScore(str, 40, i, j);
                        }

                        score+=solveThreePattern(40, i, j, 'w');

                        score += solveSingle(2, i, j, 'w');

                    }
                    else 
                    {
                        score+=calculateScore(blackFive, 1000000, i, j);

                        score+=-calculateScore(whiteFive, 1000000, i, j);

                        score+=calculateScore(blackFour, 100000, i, j);

                        score+=solveFourPattern(100000, i, j, 'b');

                        int countCaptured = capturesCount(i,j, player);
                        if(countCaptured > 0)
                        {
                            score+=100000;
                        }

                        score+=-calculateScore(whiteFour, 50000, i, j);
                        
                        score+=-solveFourPattern(50000, i, j, 'w');

                        if(whiteCaptured > blackCaptured)
                        {
                            score+=calculateScore(blackCapSequence, 500, i, j);
                        }

                        for(auto str: whiteThreeArray)
                        {
                            score+=-calculateScore(str, 500, i, j);
                        }

                        score+=-solveThreePattern(500, i, j, 'w');

                        score+= countCaptured*100;

                        for(auto str: blackThreeArray)
                        {
                            score+=calculateScore(str, 40, i, j);
                        }

                        score+=solveThreePattern(40, i, j, 'b');

                        score += solveSingle(2, i, j, 'b');

                    }
                }
            }
            return score;
        }
};

int alpha_beta(Node node, int alpha, int beta, int depth, char player) 
{
    if (depth == 0 || node.isWin(opponentPlayer) || node.isWin(ourPlayer)) 
    {
        return node.evaluate(ourPlayer);
    }

    node.generateChildren();

    if(node.children.size()==0)
    {
        return node.evaluate(ourPlayer);
    }

    if (player == ourPlayer) 
    {
        int value = -1e8;

        for (Node child: node.children) 
        {
            int temp = alpha_beta(child, alpha, beta, depth - 1, ourPlayer);
            value = max(value, temp);
            alpha = max(alpha, value);
            if (beta <= alpha) 
            {
                break;
            }
        }
        return value;

    } 
    else 
    {
        int value = 1e8;
        for (Node child: node.children) 
        {
            int temp = alpha_beta(child, alpha, beta, depth - 1, opponentPlayer);
            value = min(value, temp);
            beta = min(beta, value);
            if (beta <= alpha)
            {
                break;
            }
        }
        return value;
    }
}

pair<int, int> getPosition() 
{

    Node root(board, -1, -1, ourPlayer, 0);
    root.generateChildren();

    int best_score = -1e8;
    int best_row = -1;
    int best_col = -1;

    if(secondsLeft<20)
    {
        beta = -1e8;
    }
    else
    {
        beta = 1e8;
    }
    for (Node child: root.children) 
    {
        int score = alpha_beta(child, -1e8, beta, maxDepth, child.player);
        if (score > best_score) 
        {
            best_score = score;
            best_row = child.row;
            best_col = child.col;
        }
    }

    return make_pair(best_row, best_col);
}

void solve() 
{

  //Case 1: Empty Board and white colour
    if (ourplayer == "WHITE") 
    {

        int whiteCount = 0;
        int blackCount = 0;
        for (int i = 0; i < 19; i++) 
        {
            for (int j = 0; j < 19; j++) 
            {
                if (board[i][j] == 'w') 
                {
                    whiteCount++;
                } 
                else if (board[i][j] == 'b') 
                {
                    blackCount++;
                }
            }
        }

        if (whiteCount == 0 && blackCount == 0) 
        {
            xPos = 10;
            yPos = 'K';
            return;
        }

        //Case 2: Board has one white, one black and our player is white
        else if (whiteCount == 1 && blackCount == 1) 
        {
            if (board[9][12] == '.') 
            {
                xPos = 10;
                yPos = 'N';
                return;
            } 
            else 
            {
                xPos = 7;
                yPos = 'N';
                return;
            }
        }
    }

    //Case 3: Solve
    if (ourplayer == "WHITE") 
    {
        ourPlayer = 'w';
        opponentPlayer = 'b';
        pair<int, int> best_pos = getPosition();
        xPos = 19 - best_pos.first;
        if(65+best_pos.second>=73)
        {
            yPos = char(66+best_pos.second);
        }
        else
        {
            yPos = char(65+best_pos.second);
        }
        return;
    }
    else 
    {
        int whiteCount = 0;
        int blackCount = 0;
        for (int i = 0; i < 19; i++) 
        {
            for (int j = 0; j < 19; j++) 
            {
                if (board[i][j] == 'w') 
                {
                    whiteCount++;
                } 
                else if (board[i][j] == 'b') 
                {
                    blackCount++;
                }
            }
        }

        if(whiteCount==1 && blackCount==0)
        {
            if (board[10][10] == '.') 
            {
                xPos = 9;
                yPos = 'L';
                return;
            } 
            else 
            {
                xPos = 11;
                yPos = 'L';
                return;
            }
        }

        ourPlayer = 'b';
        opponentPlayer = 'w';
        pair<int, int> best_pos = getPosition();
        xPos = 19 - best_pos.first;
        if(65+best_pos.second>=73)
        {
            yPos = char(66+best_pos.second);
        }
        else
        {
            yPos = char(65+best_pos.second);
        }
        return;
    }

}

int main() 
{
    readFile();
    solve();
    writeFile();
    
    return 0;
}

