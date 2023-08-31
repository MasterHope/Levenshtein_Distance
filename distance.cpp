#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#define thread_num 8

using namespace std;

void printmatrix(int* mat,int m, int n){
    for(int i =0; i< m;i++){
        for(int j=0; j< n;j++){
            cout<< mat[i*n+j];
            if(j!=n-1)
                cout << ",";
        }
        cout << endl;
    }
}

int levenshteinDistance(string string1, string string2)
{
    int m = string1.length()+1;
    int n = string2.length()+1;

    int *mat = new int[m * n];

    for (int i = 0; i < m; i++)
    {
        mat[i * n] = i;
    }

    for (int j = 0; j < n; j++)
    {
        mat[j] = j;
    }


    int costoperation = 0;
    for (int j = 1; j < n; j++)
    {
        for (int i = 1; i < m; i++)
        {
            if (string1[i-1] == string2[j-1])
            {
                costoperation = 0;
            }
            else
            {
                costoperation = 1;
            }
            mat[i * n + j] = min(min(mat[(i-1) * n + j] + 1, mat[i * n + j - 1] + 1), mat[(i-1) * n + j - 1] + costoperation);
        }
    }
    
    //printmatrix(mat,m,n);
    return mat[m * n - 1];
}

int levenshteinDistanceParallel(string string1, string string2)
{
    int m = string1.length()+1;
    int n = string2.length()+1;

    int *mat = new int[m * n];
    
    #pragma omp parallel for 
    for (int i = 0; i < m; i++)
    {
        mat[i * n] = i;
    }
    #pragma omp parallel for 
    for (int j = 0; j < n; j++)
    {
        mat[j] = j;
    }
    

    for(int i=1; i < m; i++){
        #pragma omp parallel for
        for (int count = 1; count<=i;count++){
            int j_start = 0;
            int i_start = i+1;
            int costoperation = 0;
            if(j_start + count< n && i_start - count>=1){
                j_start+=count;
                i_start-=count;
                if(string1[i_start-1]==string2[j_start-1]){
                    costoperation=0;
                } else {
                    costoperation=1;
                }
                mat[i_start * n + j_start] = min(min(mat[(i_start-1) * n + j_start] + 1, mat[i_start * n + j_start - 1] + 1), mat[(i_start-1) * n + j_start - 1] + costoperation);
            }
        }
          
    }
    
    for (int j=2; j <= n; j++){
        #pragma omp parallel for
        for(int count=1; count < n;count++){
            int j_start = j-1;
            int i_start = m;
            int costoperation = 0;
            if(j_start + count< n && i_start - count>=1){
                j_start+=count;
                i_start-=count;
                if(string1[i_start-1]==string2[j_start-1]){
                    costoperation=0;
                } else {
                    costoperation=1;
                }
                mat[i_start * n + j_start] = min(min(mat[(i_start-1) * n + j_start] + 1, mat[i_start * n + j_start - 1] + 1), mat[(i_start-1) * n + j_start - 1] + costoperation);
            }
        }
    }
    //printmatrix(mat,m,n);
    return mat[m * n - 1];
}

string getStringFile(const string& path){
    ifstream input_file(path);
    if(!input_file.is_open()){
        cerr <<"Impossible to open the file in path: '" << path <<"'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int compare_two_texts(const string& path1, const string& path2){
    string text1 = getStringFile(path1);
    string text2 = getStringFile(path2);
    int start = omp_get_wtime();
    int value = levenshteinDistance(text1,text2); 
    int end = omp_get_wtime();
    cout << "Distance from two files " << value << endl;
    cout << "Duration in seconds for sequencial: " << end-start <<endl;
    start = omp_get_wtime();
    int value2 = levenshteinDistanceParallel(text1,text2);
    end = omp_get_wtime();
    cout << "Distance from two files " << value2 << endl;
    cout << "Duration in seconds for parallel: " << end-start << endl;
    return value;
}

int main(int argc, char **argv)

{
    cout << "Welcome to project_distance program! Press q to exit..." << endl;
    omp_set_dynamic(0);
    omp_set_num_threads(thread_num);
    string first_filename = "";
    string second_filename = "";
    while (true){
        cout << "Insert first text filename ";
        cin >> first_filename;     
        if(first_filename.compare("q")==0){
            break;
        }   
        cout << "Insert second text filename ";
        cin >> second_filename;
        if(second_filename.compare("q")==0){
            break;
        }   
        compare_two_texts("texts/" + first_filename,"texts/"+ second_filename);
    }
    return 0;
}
