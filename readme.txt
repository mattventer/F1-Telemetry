Open with Developer Command Prompt for VS 2017

cd to this folder, run code .

Or 

Run f1dev.bat


TODO:
	* Missing last lap sector 3 times for OSQ and some other sessions (probably before it gets to storage too)
	* Put the code in .cpp files, come on man
	* When saving multiple sessions for 1 race weekend it stores in XML as multiple weekends, 1 session per weekend
		* prior to saving it is correct. Something wrong with either saving or loading. Check storage xml
			* This might have been fixed, needs testing
	* Update CSessionHistory so the functions are not dependent on the packet version