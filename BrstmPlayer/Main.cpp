// Main.cpp
// Nolan Check
// Created 11/30/2011

#include <Windows.h>
#include <tchar.h>

#include <cstdlib>
#include <iostream>

#include "BrstmPlayer.hpp"

static bool OpenFileDialog(LPTSTR lpstrFile, DWORD nMaxFile)
{
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("BRSTM Files (*.brstm)\0*.brstm\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = nMaxFile;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (!GetOpenFileName(&ofn)) {
		return false;
	}

	return true;
}

int _tmain(int argc, TCHAR* argv[])
{
	TCHAR fileName[65536] = { 0 };
	if (!OpenFileDialog(fileName, 65536)) {
		return EXIT_FAILURE;
	}

	FilePtr file = File::Create(fileName);
	if (!file) {
		return EXIT_FAILURE;
	}

	std::wcout << "File name: " << fileName << std::endl;

	BrstmPlayerPtr player = BrstmPlayer::Create();
	if (!player) {
		return EXIT_FAILURE;
	}

	if (!player->Open(file)) {
		return EXIT_FAILURE;
	}

	if (!player->Play()) {
		return EXIT_FAILURE;
	}

	for (;;)
	{
		// Print menu
		std::cout << std::endl
			<< "Select one:" << std::endl
			<< "1) Load" << std::endl
			<< "Q) Quit" << std::endl
			<< '>';

		char choice;
		std::cin >> choice;

		switch (choice)
		{
		case '1':
			player->Stop();
			file.reset();
			if (OpenFileDialog(fileName, 65536))
			{
				file = File::Create(fileName);
				if (!file) {
					return EXIT_FAILURE;
				}
				std::wcout << "File name: " << fileName << std::endl;
				if (!player->Open(file)) {
					return EXIT_FAILURE;
				}
				if (!player->Play()) {
					return EXIT_FAILURE;
				}
			}
			break;
		case 'Q':
		case 'q':
			goto quit;
		}
	}
quit:

	player->Stop();

	return EXIT_SUCCESS;
}
