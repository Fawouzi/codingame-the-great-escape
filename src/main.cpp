#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

enum EDirection
{
    eRight = 0,
    eLeft,
    eDown
};

typedef std::pair<int, int> 		TCoordonnees;
typedef std::vector<TCoordonnees>	TVectorCoordonnees;

struct TMovesByCell
{
    typedef std::vector<TMovesByCell> 	Vector;
    typedef std::vector<Vector>			Matrix;

	bool mbUp;
	bool mbDown;
	bool mbLeft;
	bool mbRight;

	// On peut bouger dans tous les sens par défaut
	TMovesByCell () :
		mbUp 	(true),
		mbDown	(true),
		mbLeft	(true),
		mbRight (true)
	{
	}
};


struct TPath
{
	// Tableau
	int mBoard[9][9];

	TPath (void)
	{
		for (int x = 0; x < 9; x++)
		{
			for (int y = 0; y < 9; y++)
			{
				mBoard[x][y] = -1;
			}
		}
	}

	void Dump (void)
	{
		std::stringstream 	Trace;

		for (int y = 0; y < 9; y++)
		{
			for (int x = 0; x < 9; x++)
			{
				Trace << std::right << std::setw(3) << std::setfill(' ') << mBoard[x][y] << " ";
			}

			Trace << "\r\n";
		}

		cerr << Trace.str().c_str() << endl;
	}

	void ProchaineCase (int aX, int aY, int aDistance, TVectorCoordonnees& aVectorCoordonnees)
	{
		// La case existe et elle n'avait pas été visitée
		if ((0 <= aX) && (8 >= aX) && (0 <= aY) && (8 >= aY) && (-1 == mBoard[aX][aY]))
		{
			// Maj de la distance
			mBoard[aX][aY] = aDistance;

			// Ajout de la case aux prochaines à evaluer
			aVectorCoordonnees.push_back(make_pair(aX, aY));
		}
	}

	void Compute (int aX, int aY, TMovesByCell::Matrix& aBoard)
	{
        cerr << "Compute - aX : " << aX << " - aY : " << aY  << endl;

		int distance = 0;
		int casesVisitees = 81;
		TVectorCoordonnees	VectorCoordonnees;

		// Init de la position
		mBoard[aX][aY] = distance;
		casesVisitees--;
		distance++;
		VectorCoordonnees.push_back(std::make_pair(aX, aY));

		while (casesVisitees)
		{
			TVectorCoordonnees				VectorCoordonneesResultat;
			TVectorCoordonnees::iterator  	iVecCoord;

			// Parcours des coordonnées obtenues précédemment
			for (iVecCoord = VectorCoordonnees.begin(); iVecCoord != VectorCoordonnees.end(); iVecCoord++)
			{
				int X = iVecCoord->first;
				int Y = iVecCoord->second;

				if (aBoard[X][Y].mbLeft)
				{
					ProchaineCase (X - 1, Y, distance, VectorCoordonneesResultat);	// Gauche
				}
				if (aBoard[X][Y].mbRight)
				{
					ProchaineCase (X + 1, Y, distance, VectorCoordonneesResultat);	// Droite
				}
				if (aBoard[X][Y].mbUp)
				{
					ProchaineCase (X, Y - 1, distance, VectorCoordonneesResultat);	// Haut
				}
				if (aBoard[X][Y].mbDown)
				{
					ProchaineCase (X, Y + 1, distance, VectorCoordonneesResultat);	// Bas
				}
			}

			// Le nombre de cases visitées diminue autant que de résultats
			casesVisitees -= VectorCoordonneesResultat.size();

			// La distance augmente
			distance++;

			// Sauve pour la prochaine itération
			VectorCoordonnees = VectorCoordonneesResultat;
		}
	}
};

struct TPlayer
{
    typedef std::vector<TPlayer> Vector;

    int 			mId;
    int 			mX;
    int 			mY;
    int 			mWallsLeft;
    EDirection		mDirection;
    TPath			mPath;

    TPlayer (int aId, int aX, int aY, int aWallsLeft) :
        mId         (aId),
        mX          (aX),
        mY          (aY),
        mWallsLeft  (aWallsLeft)
    {
    	switch (mId)
		{
			case 0: mDirection = eRight; 	break;
			case 1: mDirection = eLeft;  	break;
			case 2: mDirection = eDown;  	break;
			default:
				cerr << "TPlayer : error sw/case" << endl;
		}
    }

    bool ChercheNextMove (int aX, int aY, int aDistance)
    {
    	bool bFound = false;

    	// La case existe
    	if ((0 <= aX) && (8 >= aX) && (0 <= aY) && (8 >= aY))
    	{
    		if ((aDistance - 1) == mPath.mBoard[aX][aY])
    		{
    			bFound = true;
    		}
    	}

    	return bFound;
    }

    void ComputePath (TMovesByCell::Matrix& aBoard)
    {
    	mPath.Compute (mX, mY, aBoard);
    }
};


struct TWall
{
    typedef std::vector<TWall> Vector;

    int 		mX;
    int 		mY;
    std::string mOrientation;

    TWall (int aX, int aY, const std::string& aOrientation) :
        mX          	(aX),
        mY          	(aY),
		mOrientation	(aOrientation)
    {
    }
};

void Dump (TPlayer::Vector& aPlayerVector, TWall::Vector& aWallVector)
{
	cerr << "************* Players ********************" << endl;
	TPlayer::Vector::iterator iPlayer;
	for (iPlayer = aPlayerVector.begin(); iPlayer != aPlayerVector.end(); iPlayer++)
	{
       	cerr 	<< "Player : " << iPlayer->mId << " - X : " << iPlayer->mX << " - Y : " << iPlayer->mY
       			<< " - WallsLeft : " << iPlayer->mWallsLeft << endl;
	}

	cerr << "************* Walls **********************" << endl;
	TWall::Vector::iterator iWall;
	for (iWall = aWallVector.begin(); iWall != aWallVector.end(); iWall++)
	{
       	cerr 	<< "Wall " << " - X : " << iWall->mX << " - Y : " << iWall->mY
       			<< " - Orientation : " << iWall->mOrientation << endl;
	}
}

void InitBoard (TMovesByCell::Matrix& aBoard, int aWidth, int aHeight)
{
    // Resize de X (width)
    aBoard.resize (aWidth);

    // Resize de Y (height)
    for (int i = 0; i < aWidth; i++)
    {
    	aBoard[i].resize (aHeight);
    }
}

void MajBoard (TMovesByCell::Matrix& aBoard, TWall::Vector& aWallVector, int aWidth, int aHeight)
{
	TWall::Vector::iterator iWall;
	for (iWall = aWallVector.begin(); iWall != aWallVector.end(); iWall++)
	{
		// Vertical
		if ("V" == iWall->mOrientation)
		{
			// Impossible de couper par la droite sur deux cases de haut
			if (0 <= (iWall->mX - 1))
			{
				aBoard[iWall->mX - 1][iWall->mY]	.mbRight = false;
				aBoard[iWall->mX - 1][iWall->mY + 1].mbRight = false;
			}

			// Impossible de couper par la gauche sur deux cases de haut
			aBoard[iWall->mX][iWall->mY]	.mbLeft = false;
			aBoard[iWall->mX][iWall->mY + 1].mbLeft = false;
		}

		// Horizontal
		else
		{
			// Impossible de couper par le bas sur deux cases de large
			if (0 <= (iWall->mY - 1))
			{
				aBoard[iWall->mX    ][iWall->mY - 1].mbDown = false;
				aBoard[iWall->mX + 1][iWall->mY - 1].mbDown = false;
			}

			// Impossible de couper par le haut sur deux cases de large
			aBoard[iWall->mX    ][iWall->mY].mbUp = false;
			aBoard[iWall->mX + 1][iWall->mY].mbUp = false;
		}
	}
}

void MoveMyPlayer (TPlayer& aPlayer)
{
	TCoordonnees	MeilleureSortie;
	TCoordonnees	NextMove;
	int distance =  999;	// Initialise à une très grande valeur pour la sélection

	// TODO FAB : factoriser

	switch (aPlayer.mDirection)
	{
		case (eRight) :
			for (int y = 0; y < 9; y++)
			{
				if (distance > aPlayer.mPath.mBoard[8][y])
				{
					distance = aPlayer.mPath.mBoard[8][y];
					MeilleureSortie = make_pair(8, y);
				}
			}
		break;

		case (eLeft) :
			for (int y = 0; y < 9; y++)
			{
				if (distance > aPlayer.mPath.mBoard[0][y])
				{
					distance = aPlayer.mPath.mBoard[0][y];
					MeilleureSortie = make_pair(0, y);
				}
			}
		break;

		case (eDown) :
			for (int x = 0; x < 9; x++)
			{
				if (distance > aPlayer.mPath.mBoard[x][8])
				{
					distance = aPlayer.mPath.mBoard[x][8];
					MeilleureSortie = make_pair(x, 8);
				}
			}
		break;
	}

	// Initialise le next move
	NextMove = MeilleureSortie;

	while (distance > 1)
	{
		// Gauche
		if (aPlayer.ChercheNextMove (NextMove.first - 1, NextMove.second, distance))
		{
			NextMove.first--;
		}
		// Droite
		else if (aPlayer.ChercheNextMove (NextMove.first + 1, NextMove.second, distance))
		{
			NextMove.first++;
		}
		// Haut
		else if (aPlayer.ChercheNextMove (NextMove.first, NextMove.second - 1, distance))
		{
			NextMove.second--;
		}
		// Bas
		else
		{
			NextMove.second++;
		}

		distance--;
	}

	// Compare NextMove à la position courante

	// Gauche
	if ((NextMove.first == aPlayer.mX - 1) && (NextMove.second == aPlayer.mY))
	{
		cout << "LEFT" << endl;
	}
	// Droite
	if ((NextMove.first == aPlayer.mX + 1) && (NextMove.second == aPlayer.mY))
	{
		cout << "RIGHT" << endl;
	}
	// Haut
	if ((NextMove.first == aPlayer.mX) && (NextMove.second == aPlayer.mY - 1))
	{
		cout << "UP" << endl;
	}
	// Bas
	if ((NextMove.first == aPlayer.mX) && (NextMove.second == aPlayer.mY + 1))
	{
		cout << "DOWN" << endl;
	}

	cerr 	<< "MoveMyPlayer - MeilleureSortie : (" << MeilleureSortie.first << "," << MeilleureSortie.second << ")"
			<< " - NextMove : (" << NextMove.first << "," << NextMove.second << ")" << endl;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int w; 				// width of the board
    int h; 				// height of the board
    int playerCount; 	// number of players (2 or 3)
    int myId; 			// id of my player (0 = 1st player, 1 = 2nd player, ...)
    cin >> w >> h >> playerCount >> myId; cin.ignore();

    TPlayer::Vector	PlayerVector;
    TWall::Vector	WallVector;

    TMovesByCell::Matrix	Board;

    // Initialise le board avec les valeurs initiales
    InitBoard (Board, w, h);

    // game loop
    while (1)
    {
    	// Reset des tableaux
    	PlayerVector.clear();
    	WallVector.clear();

    	// Parcours des joueurs
        for (int i = 0; i < playerCount; i++)
        {
            int x; 			// x-coordinate of the player
            int y; 			// y-coordinate of the player
            int wallsLeft; 	// number of walls available for the player
            cin >> x >> y >> wallsLeft; cin.ignore();

			// Peuplement du tableau des joueurs
			PlayerVector.push_back (TPlayer (i, x, y, wallsLeft));
        }


        int wallCount; // number of walls on the board
        cin >> wallCount; cin.ignore();
        for (int i = 0; i < wallCount; i++)
        {
            int wallX; 				// x-coordinate of the wall
            int wallY; 				// y-coordinate of the wall
            string wallOrientation; // wall orientation ('H' or 'V')
            cin >> wallX >> wallY >> wallOrientation; cin.ignore();

            // Peuplement du tableau des murs
            WallVector.push_back (TWall (wallX, wallY, wallOrientation));
        }

        // Maj du board suite maj du nombre de murs
        cerr << "MajBoard" << endl;
        MajBoard (Board, WallVector, w, h);

        // Calcule les possibilités de déplacement
        cerr << "ComputePath" << endl;
        /*TPlayer::Vector::iterator iPlayer;
	    for (iPlayer = PlayerVector.begin(); iPlayer != PlayerVector.end(); iPlayer++)
	    {
            iPlayer->ComputePath (Board);
	    }*/
	    PlayerVector[myId].ComputePath (Board);

        // Dump des tableaux
        cerr << "Dump" << endl;
        Dump (PlayerVector, WallVector);
    	cerr << "************* Path **********************" << endl;
        PlayerVector[myId].mPath.Dump();

        // Bouge mon joueur
        MoveMyPlayer (PlayerVector[myId]);

        //cout << "RIGHT" << endl; // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
    }
}
