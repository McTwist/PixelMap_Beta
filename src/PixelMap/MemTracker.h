// MemTracker - simple class for checking memory leaks
// NOTE: only works in DEBUG mode
#pragma once
#include <windows.h>
#include <crtdbg.h>



// Constants
#define MEMTRACKER_MAX_CHECKPOINT	16


// Class declarations
class MemTracker
{
public:
	// --------------------------------------------------------------------
	// MemTracker - opens file and attaches it to the debug system
	// --------------------------------------------------------------------
	MemTracker( char *filename = "memleak.wri" )
	{
		// Open report file
		m_file = CreateFileA( filename, 
			GENERIC_WRITE, 
			0, 0, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			0 );

		// Write hello message in log
		DWORD temp = 0;
		char str[] = {"MemTracker: The following leaks were found:\n"};
		WriteFile( m_file, str, sizeof(str)-1, &temp, 0 );
		FlushFileBuffers( m_file );


		// Attach file to debug system
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_WARN, m_file );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ERROR, m_file );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ASSERT, m_file );

		setCheckPoint(0);
	}


	// --------------------------------------------------------------------
	// ~MemTracker - closes file
	// --------------------------------------------------------------------
	~MemTracker()
	{
		setCheckPoint(1);
		dumpReportToFile(0,1);

		// Write goodbye message in log
		DWORD temp = 0;
		char str[] = {"MemTracker exited.\n"};
		WriteFile( m_file, str, sizeof(str)-1, &temp, 0 );
		FlushFileBuffers( m_file );


		// Close report file
		CloseHandle( m_file );
	}


	// --------------------------------------------------------------------
	// setCheckPoint - saves a memory state for later comparison
	// --------------------------------------------------------------------
	void setCheckPoint( int index )
	{
		// Sanity check
		if( ( index >= 0 ) && ( index < MEMTRACKER_MAX_CHECKPOINT ) )
			_CrtMemCheckpoint( &m_memStates[ index ] );
	}


	// --------------------------------------------------------------------
	// memStillAllocated - true if there is any difference between the
	//                     checkpoints specified by the indices
	// --------------------------------------------------------------------
	BOOL memStillAllocated( int firstIndex, int secondIndex )
	{
		// Returns true if difference
		return _CrtMemDifference(	&m_tempState,	// Result, difference
			&m_memStates[ firstIndex ],
			&m_memStates[ secondIndex ] );
	}


	// --------------------------------------------------------------------
	// dumpReportToFile - writes memory statistics to report file,
	//					  if there is a difference between the checkpoints
	// --------------------------------------------------------------------
	void dumpReportToFile( int firstIndex, int secondIndex )
	{
		_CrtMemDifference(	&m_tempState,	// Result, difference
			&m_memStates[ firstIndex ],
			&m_memStates[ secondIndex ] );

		_CrtMemDumpStatistics( &m_tempState );

		FlushFileBuffers( m_file );
	}




private:
	HANDLE m_file;

	_CrtMemState m_memStates[ MEMTRACKER_MAX_CHECKPOINT ];
	_CrtMemState m_tempState;
};
