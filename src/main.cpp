#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct TPlayer
{
    typedef std::vector<TPlayer> Vector;

    int mId;
    int mX;
    int mY;
    int mWallsLeft;

    TPlayer (int aId, int aX, int aY, int aWallsLeft) :
        mId         (aId),
        mX          (aX),
        mY          (aY),
        mWallsLeft  (aWallsLeft)
    {
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

    // TODO FAB : Adapter en fonction du nb de joueurs

    // Impossible de sortir par le haut ou le bas
    for (int x = 0; x < aWidth; x++)
    {
    	aBoard[x][0].mbUp = false;
    	aBoard[x][8].mbDown = false;
    }

    // Impossible de sortir par la gauche
    for (int y = 0; y < aHeight; y++)
    {
    	aBoard[0][y].mbLeft = false;
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


void MoveMyPlayer (TMovesByCell::Matrix& aBoard, TPlayer& aPlayer, int aWidth, int aHeight)
{
	bool bRightOk = true;
	int iRight;

	for (iRight = 0; iRight < (aWidth - aPlayer.mX); iRight++)
	{
		if (!aBoard[aPlayer.mX + iRight][aPlayer.mY].mbRight)
		{
			bRightOk = false;
			break;
		}
	}

	// On privilégie toujours d'aller à droite si possible
	if (bRightOk)
	{
		cout << "RIGHT" << endl;
	}
	// Nous sommes bloqués, donc ...
	else
	{
		int iUp;
		int iDown;

		for (iUp = 0; iUp < (aHeight - aPlayer.mY); iUp++)
		{
			if (aBoard[aPlayer.mX][aPlayer.mY - iUp].mbRight)
			{
				break;
			}
		}

		for (iDown = 0; iDown < (aHeight - aPlayer.mY); iDown++)
		{
			if (aBoard[aPlayer.mX][aPlayer.mY + iDown].mbRight)
			{
				break;
			}
		}

		if (iDown < iUp)
		{
			// Si c'est possible d'aller en bas, on le fait sinon on recule
			if (aBoard[aPlayer.mX][aPlayer.mY].mbDown)
			{
				cout << "DOWN" << endl;
			}
			else
			{
				cout << "LEFT" << endl;
			}
		}
		else
		{
			// Si c'est possible d'aller en haut, on le fait sinon on recule
			if (aBoard[aPlayer.mX][aPlayer.mY].mbUp)
			{
				cout << "UP" << endl;
			}
			else
			{
				cout << "LEFT" << endl;
			}
		}
	}
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
        MajBoard (Board, WallVector, w, h);

        // Dump des tableaux
        Dump (PlayerVector, WallVector);

        // Acquisition mon joueur
        TPlayer MyPlayer = PlayerVector[myId];

        // Bouge mon joueur en conséquence des murs qui ont été posés
        MoveMyPlayer (Board, MyPlayer, w, h);

        //cout << "RIGHT" << endl; // action: LEFT, RIGHT, UP, DOWN or "putX putY putOrientation" to place a wall
    }
}
