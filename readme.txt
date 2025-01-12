Open with Developer Command Prompt for VS 2017

cd to this folder, run code .

Or 

Run f1dev.bat


TODO:
	* Put the code in .cpp files, come on man
	* Figure out why opening any race that is not in the .xml causes a crash
	* Starting a new race/practice on a track that already exists in storage results in the new session being appended to the previous race weekend, instead of creating a new one
		* This was observed prior to clicking save, so when starting a new session (while only in memory, before storing in xml) there is something wrote (not checking uid? just trackname probably) 