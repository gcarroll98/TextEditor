#define PDC_DLL_BUILD 1
#include "curses.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;





int main(int argc,char* argv[])
{
	
	WINDOW* main_window = nullptr;
	WINDOW* sub_window = nullptr;
	int num_rows = 0;
	int num_cols = 0;
	
	
	//start a new window for editing text.
	
	main_window= initscr();
	

	/*
	In class notes for grabbing text from file
	

	to pull string from vector use
	wmvaddstr(my_str.c_str());
	
	*/



	//take up most of the screen
	getmaxyx(main_window, num_rows, num_cols);
	int sub_rows = num_rows - 3;
	int sub_cols = num_cols - 2;

	//Put a new subwindow ontop of main window, so it is separate from the border and exit statement
	sub_window = newwin(sub_rows, sub_cols, 2, 1);
	

	vector<char> indline(0);
	vector<vector<char>> lines(0);

	//resize 2d vector to the the same size as the window
	//lines.resize(num_cols, vector<char>(num_rows));
	
	  
	
	

	keypad(sub_window, TRUE);
	//nodelay(sub_window, TRUE);
	cbreak();
	curs_set(2);
	//scrollok(sub_window, TRUE);
	

	wborder(main_window, 0, 0, 0, 0, 0, 0, 0, 0);
	
	mvwaddstr(main_window,1, 1, "Press ESC to exit");
	wrefresh(main_window);
	mvwaddstr(sub_window,0, 0, "Enter name of text file: ");
	wrefresh(sub_window);
	
	wmove(sub_window,1, 0);

	
	int x, y;
	char filename[70];

	wmove(sub_window, 1, 0);
	
	//Get the name of the text file to be opened
	wgetstr(sub_window, filename);

	//Open the text file input from the screen
	ifstream txtfile;
	txtfile.open(filename);
	char filechars;
	
	wmove(sub_window, 2, 0);

	//while there is contents in the file
	while (txtfile.good() == TRUE) {

		//get individual characters from text file
		txtfile.get(filechars);

		//push characters into vector for each individual line
		indline.push_back(filechars);

		//check to see if at end of line in text file
		if (filechars == '\n') {

			//store the individual line into the vector of char vectors
			lines.push_back(indline);

			//clear the chars out of individual line to make room for next line
			indline.clear();
		}
	}
	
	//add last line of text to lines
	lines.push_back(indline);

	//close text file after grabbing everything inside
	txtfile.close();

	int	uppbnd = 0; //upper bound of vector to be displayed on screen. Initialize to 0.
	int lowbnd = lines.size(); //lower bound of vector to be displayed on screen. Starts at bottom of vector

	//Output contents to screen

	//First check if file length is larger than screen size
	if (lines.size() >= sub_rows-1) {
		//Clear text already on screen
		wclear(sub_window);
		
		//Find the point in the text file vector where you can display the bottom portion of text on one screen.
		do {
			uppbnd = uppbnd + sub_rows;
		} while (uppbnd<(lowbnd-sub_rows));

		//Output bottom portion of text file to screen
		for (int i = uppbnd; i < lines.size(); i++) {
			
			// set parameter to length of char vector i in vector lines
			for (int j = 0; j < lines[i].size(); j++) {
				//add chars to screen
				waddch(sub_window, lines[i][j]);
			}
		}

	}
	//Otherwise output contents normally 
	else {
		//output 2d vector to sub window
		for (int i = 0; i < lines.size(); i++) {
			// set parameter to length of char vector i in vector lines
			for (int j = 0; j < lines[i].size(); j++) {
				//add chars to screen
				waddch(sub_window, lines[i][j]);
			}
		}
	}
	wrefresh(sub_window);
	
	int remaining = 0; // number of remaining lines below lower bound

	int result = ' ';
	noecho();
	while (result != 27) {
		int result2 = wgetch(sub_window);
		
		getyx(sub_window, y, x);

		

		switch (result2) {
		case KEY_DOWN:
			if (y == sub_rows-1 && remaining > 0) {
				wclear(sub_window);
				uppbnd = uppbnd + sub_rows;
				if (lowbnd + sub_rows >= lines.size()) {
					lowbnd = lines.size();
					for (int i = uppbnd; i < lowbnd; i++) {
						// set parameter to length of char vector i in vector lines
						for (int j = 0; j < lines[i].size(); j++) {
							//add chars to screen
							waddch(sub_window, lines[i][j]);
						}
					}
				}
					
			else {
				lowbnd = lowbnd + sub_rows;
				for (int i = uppbnd; i < uppbnd+sub_rows; i++) {

					// set parameter to length of char vector i in vector lines
					for (int j = 0; j < lines[i].size(); j++) {
						//add chars to screen
						waddch(sub_window, lines[i][j]);
						}
					}
				}
				
				remaining = lines.size() - lowbnd;
				wmove(sub_window, 0, 0);
			}
			else
				wmove(sub_window, y + 1, x);
			
			break;
		case KEY_UP:
			if (y == 0 && uppbnd >= sub_rows) {
				//Clear current text on screen
				wclear(sub_window);
			
				//Scroll up full screen length of vector
				uppbnd = uppbnd - sub_rows;
				lowbnd = lowbnd - sub_rows;
				remaining = lines.size() - lowbnd;
				for (int i = uppbnd; i < uppbnd + sub_rows; i++) {

					// set parameter to length of char vector i in vector lines
					for (int j = 0; j < lines[i].size(); j++) {
						//add chars to screen
						waddch(sub_window, lines[i][j]);
					}
				}
			}
			else if (y == 0) {
				wclear(sub_window);
				

				for (int i = 0; i < sub_rows; i++) {

					// set parameter to length of char vector i in vector lines
					for (int j = 0; j < lines[i].size(); j++) {
						//add chars to screen
						waddch(sub_window, lines[i][j]);
					}
				}
			}
			else
			{
				wmove(sub_window, y - 1, x);

			}
			break;
		case KEY_LEFT:
			//If at far left of sub window, move cursor to the end of previous line
			if (x == 0) { 
				wmove(sub_window, y - 1, sub_cols);
			}
			// Move cursor one to position to left
			else 
			{
				wmove(sub_window, y, x - 1);
			}
			
			  
			break;
		case KEY_RIGHT: 
			//If at far right of sub window, move cursor to the front of next line
			if (x == sub_cols) {
				wmove(sub_window, y + 1, 0);
			}
			// Move cursor one position to right
			else {
				wmove(sub_window, y, x + 1);
			}
			break;
		case 8:
			
				wprintw(sub_window,"\b");
				wprintw(sub_window," ");
				waddch(sub_window,result2);
			
			break;
		case 10:
			if (y < sub_rows-1)
			{
				
							
				y++;
				wmove(sub_window,y, 0);
			}
			break;
		default:
			waddch(sub_window,result2);
			
			break;
		

		}
		
		if (x == sub_cols && result2 != 8)
			wmove(sub_window,y+1, 0);
		
		
		
		
		
		
			result = result2;
	}
	lines.clear();
	indline.clear();
	endwin();
	

}
