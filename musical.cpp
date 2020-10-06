/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *
 * Program: Musical chairs game with n players and m intervals.
 * Author:   T Sandeep Kumar and K Sai Krishna 
 * Roll# :  CS18BTECH11025 & CS18BTECH11044
 *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

#include <stdlib.h>  /* for exit, atoi */
#include <iostream>  /* for fprintf */
#include <errno.h>   /* for error code eg. E2BIG */
#include <getopt.h>  /* for getopt */
#include <assert.h>  /* for assert */
#include <chrono>	/* for timers */
#include <thread>   /*for threads*/
#include <cstring>  /*for string comparision*/
#include <mutex>    /*for sync*/
/*
 * Forward declarations
 */

void usage(int argc, char *argv[]);
unsigned long long musical_chairs(int nplayers);

using namespace std;
int count;
char *s;//input string
bool run = 0;//music_stop
bool listen = 0;//music_start
mutex mtx1, mtx2;
int nplayers;
int pplayers;
int pl_delay[1000] = {0};
std::thread player[1000];

int main(int argc, char *argv[])
{
    int c;

    /* Loop through each option (and its's arguments) and populate variables */
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"help",            no_argument,        0, 'h'},
            {"nplayers",         required_argument,    0, '1'},
            {0,        0,            0,  0 }
        };

        c = getopt_long(argc, argv, "h1:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            cerr << "option " << long_options[option_index].name;
            if (optarg)
                cerr << " with arg " << optarg << endl;
            break;

        case '1':
            nplayers = atoi(optarg);
            break;

        case 'h':
        case '?':
            usage(argc, argv);

        default:
            cerr << "?? getopt returned character code 0%o ??n" << c << endl;
            usage(argc, argv);
        }
    }

    if (optind != argc) {
        cerr << "Unexpected arguments.\n";
        usage(argc, argv);
    }


	if (nplayers == 0) {
		cerr << "Invalid nplayers argument." << endl;
		return EXIT_FAILURE;
	}
    pplayers = nplayers;
    unsigned long long game_time;
	game_time = musical_chairs(nplayers);

    cout << "Time taken for the game: " << game_time << " us" << endl;

    exit(EXIT_SUCCESS);
}

/*
 * Show usage of the program
 */
void usage(int argc, char *argv[])
{
    cerr << "Usage:\n";
    cerr << argv[0] << "--nplayers <n>" << endl;
    exit(EXIT_FAILURE);
}

void umpire_main(int nplayers)
{
    /* Add your code here */
    while (pplayers)
    {
        cin >> s;
        if (strcmp(s, "lap_start") == 0){
            cout<<"======= lap# "<< nplayers - pplayers + 1 <<" ======="<<endl;
            fflush(stdout);
        }
        
        if (strcmp (s, "player_sleep") == 0)
        {   
            int delay, plid;
            cin >> plid >> delay;
            pl_delay[plid] = delay;
        }
        
        if (strcmp(s, "music_start") == 0)
        {
            listen = 1;
        }
        
        if (strcmp (s, "umpire_sleep") == 0)
        {
            int delay;
            cin >> delay;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        if (strcmp(s, "music_stop") == 0)
        {
            run = 1;
        }

        if (strcmp(s, "lap_stop") == 0 && --pplayers){
            //mtx2.lock();
            
            run = 0;
            cout << "**********************" << endl;
            fflush(stdout);

            if (pplayers == 0)
            {
                cout << "Winner is"  << endl;            
                fflush(stdout);
                return;
            }
            
            //mtx2.unlock();
        }
    }
    return;
    /* read stdin only from umpire */
}

void player_main(int plid)
{
    /* Add your code here */
    while(pplayers != 0){
        if (listen)
        {
            if (pl_delay[plid])
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pl_delay[plid]));        
                pl_delay[plid] = 0;
            }
        }
        if (run)//music_stop activates
        {
            unique_lock<mutex> lock1(mtx1);
            if (count < pplayers)
            {
                count ++;
            }
            else
            {   
                mtx2.lock();
                cout << plid <<" could not get chair" << endl;
                fflush(stdout);
                mtx2.unlock();
            }
            mtx1.unlock();
        }
    }
    /* synchronize stdouts coming from multiple players */
	return;
}

unsigned long long musical_chairs(int nplayers)
{
    cout << "Musical Chairs: "<< nplayers << " player game with "<< nplayers-1 <<" laps."<<endl;
    auto t1 = chrono::steady_clock::now();

	// Spawn umpire thread.
    /* Add your code here */
    thread umpire(umpire_main, nplayers);
	
    // Spawn n player threads.
    /* Add your code here */
    for (int i = 0; i < nplayers; i++)
    {
       player[i] = thread (player_main, i);
    }
    umpire.join();
    for (int i = 0; i < nplayers; i++)
    {
        player[i].join();
    }

	auto t2 = chrono::steady_clock::now();

	auto d1 = chrono::duration_cast<chrono::microseconds>(t2 - t1);

	return d1.count();
}

