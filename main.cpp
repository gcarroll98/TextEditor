#define PDC_DLL_BUILD 1
#include "curses.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


void storescreen(WINDOW* sub, int top, int rows, int cols);


WINDOW* main_window = nullptr;
WINDOW* sub_window = nullptr;
int num_rows = 0;
int num_cols = 0;
int sub_cols;
int sub_rows;

vector<char> indline(0);
vector<vector<char>> lines(0);

int main(int argc,char* argv[])
{
	
	//start a new window for editing text.
	main_window = initscr();

	
	
	//take up most of the screen
	getmaxyx(main_window, num_rows, num_cols);
	int sub_rows = num_rows - 3;
	int sub_cols = num_cols - 2;

	//Put a new subwindow ontop of main window, so it is separate from the border and exit statement
	sub_window = newwin(sub_rows, sub_cols, 2, 1);
	

	keypad(sub_window, TRUE);
	//nodelay(sub_window, TRUE);
	cbreak();
	curs_set(2);
	//scrollok(sub_window, TRUE);
	
	//Set border in main window
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
	char filechars = ' ';
	char lastfilechar = ' ';
	wmove(sub_window, 2, 0);

	//while there is contents in the file
	while (txtfile.good() == TRUE) {
		lastfilechar = filechars;
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

	//OUTPUT CONTENTS OF FILE TO SCREEN

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
		wclear(sub_window);
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
	int result = ' '; //initialize variable for catching keyboard input
	
	noecho();

	//GET KEYBOARD INPUT FROM USER FOR EDITING/SCROLLING

	while (result != 27) {

		//get keystroke value from subwindow
		int result2 = wgetch(sub_window);
		
		//find position on screen
		getyx(sub_window, y, x);

	
		switch (result2) {

		case KEY_DOWN:
			if (y == sub_rows - 1 && remaining > 0) {
				//when scrolling, save contents on screen before going to next page of text
				storescreen(sub_window, uppbnd, sub_rows, sub_cols);

				//clear previous contents of screen
				wclear(sub_window);

				//set new placeholder for where we are in vector one full screen length down
				uppbnd = uppbnd + sub_rows;

				//check if lower bound would go out of scope
				if (lowbnd + sub_rows >= lines.size()) {

					//set lower bound to end of vector length
					lowbnd = lines.size();

					//start at top of vector placeholder
					for (int i = uppbnd; i < lowbnd; i++) {
						// set parameter to length of char vector i in vector lines
						for (int j = 0; j < lines[i].size(); j++) {
							//add chars to screen
							waddch(sub_window, lines[i][j]);
						}
					}
					

				}
			
					
				else {
					storescreen(sub_window, uppbnd, sub_rows, sub_cols);

					lowbnd = lowbnd + sub_rows;
					for (int i = uppbnd; i < uppbnd + sub_rows; i++) {

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
				
				storescreen(sub_window,uppbnd, sub_rows,sub_cols);

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
				
				storescreen(sub_window, uppbnd, sub_rows,sub_cols);

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

			//text wrapping with indicator '-' if word goes to next line
			
			if (x == sub_cols - 1 && result2 != 8 && result2 != 32 && result != 32) {
				waddch(sub_window, '-');
				wmove(sub_window, y + 1, 0);
			}
			else if (x == sub_cols - 1 && result2 != 8) {
				wmove(sub_window, y + 1, 0);
			}
			
			waddch(sub_window,result2);
			
			break;
		

		}
		
		
		//set keyboard stroke equal to result to check for ESC
			result = result2;
	}

	//close window
	endwin();
	/*
	//Output vector contents back to file
	ofstream outfile;
	outfile.open(filename);

	for (int i = 0; i < lines.size(); i++) {
		for (int j = 0; j < lines[i].size(); j++) {
			outfile << lines[i][j];
		}
	}

	outfile.close();
	*/
	lines.clear();
	indline.clear();
}
	

//Function to store the current screen contents back in the vector
void storescreen(WINDOW* sub, int top, int rows, int cols) {
	int y = 0;
	int bottom = top + rows;
	wmove(sub, 0, 0);
	indline.clear();
	for (int i = top; i < bottom && i < lines.size(); i++) {
		for (int j = 0; j < cols; j++) {
			
			
				char ch = char(mvwinch(sub, y, j));
				indline.push_back(ch);
				
			
			
		}
		y++;
		
		lines[i] = indline;
		indline.clear();

	}
	

}