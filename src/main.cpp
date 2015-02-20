#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

enum EDirection
{
    eRight = 0,
    eLeft,
    eDown
};

typedef pair<int, int> 			TCoordonnees;
typedef vector<TCoordonnees>	TVectorCoordonnees;

struct TMovesByCell
{
   typedef vector<TMovesByCell> 	Vector;
   typedef vector<Vector>			   Matrix;

	bool mbUp;
	bool mbDown;
	bool mbLeft;
	bool mbRight;

	// On peut bouger dans tous les sens par d�faut
	TMovesByCell () :
		mbUp 	(true),
		mbDown	(true),
		mbLeft	(true),
		mbRight (true)
	{
	}

	const char* Dump (void)
	{
	   stringstream   Trace;

	   Trace
	      << "U" << (mbUp ? "1" : "0")
         << "D" << (mbDown ? "1" : "0")
         << "L" << (mbLeft ? "1" : "0")
         << "R" << (mbRight ? "1" : "0");

	   return Trace.str().c_str();
	}
};

void DumpMatrix (TMovesByCell::Matrix& aMatrix)
{
   stringstream   Trace;

   for (int y = 0; y < 9; y++)
   {
      for (int x = 0; x < 9; x++)
      {
         Trace << aMatrix[x][y].Dump() << " ";
      }

      Trace << "\r\n";
   }

   cerr << Trace.str().c_str() << endl;
}

struct TPath
{
	// Tableau
	int mBoard[9][9];

	TPath (void)
	{
	   Reset ();
	}

	void Reset (void)
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
		stringstream 	Trace;

		for (int y = 0; y < 9; y++)
		{
			for (int x = 0; x < 9; x++)
			{
				Trace << right << setw(3) << setfill(' ') << mBoard[x][y] << " ";
			}

			Trace << "\r\n";
		}

		cerr << Trace.str().c_str() << endl;
	}

	void ProchaineCase (int aX, int aY, int aDistance, TVectorCoordonnees& aVectorCoordonnees)
	{
		// La case existe et elle n'avait pas �t� visit�e
		if ((0 <= aX) && (8 >= aX) && (0 <= aY) && (8 >= aY) && (-1 == mBoard[aX][aY]))
		{
			// Maj de la distance
			mBoard[aX][aY] = aDistance;

			// Ajout de la case aux prochaines � evaluer
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
		VectorCoordonnees.push_back(make_pair(aX, aY));

		while (		casesVisitees 				// Il reste des cases � visiter
				&& !VectorCoordonnees.empty())	// Il y a des candidats accessibles (v�rifie que les murs ne nous bloquent pas)
		{
			TVectorCoordonnees				VectorCoordonneesResultat;
			TVectorCoordonnees::iterator  	iVecCoord;

			// Parcours des coordonn�es obtenues pr�c�demment
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

			// Le nombre de cases visit�es diminue autant que de r�sultats
			casesVisitees -= VectorCoordonneesResultat.size();

			// La distance augmente
			distance++;

			// Sauve pour la prochaine it�ration
			VectorCoordonnees = VectorCoordonneesResultat;
		}
	}
};

struct TPlayer
{
    typedef vector<TPlayer> Vector;

    int 			mId;
    int 			mX;
    int 			mY;
    int 			mWallsLeft;
    EDirection		mDirection;
    TPath			mPath;
	TCoordonnees	mMeilleureSortie;
	int 			mDistance;
	int 			mDistancePond;

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

    void ResetPath (void)
    {
       mPath.Reset();
    }

    void ComputePath (TMovesByCell::Matrix& aBoard)
    {
    	mPath.Compute (mX, mY, aBoard);
    }

    void ComputeDistance (void)
    {
    	TCoordonnees	MeilleureSortie;
    	int distance =  999;	// Initialise � une tr�s grande valeur pour la s�lection

    	switch (mDirection)
    	{
    		case (eRight) :
    			for (int y = 0; y < 9; y++)
    			{
    				if ((distance > mPath.mBoard[8][y]) && (mPath.mBoard[8][y] != -1))
    				{
    					distance = mPath.mBoard[8][y];
    					MeilleureSortie = make_pair(8, y);
    				}
    			}
    		break;

    		case (eLeft) :
    			for (int y = 0; y < 9; y++)
    			{
    				if ((distance > mPath.mBoard[0][y]) && (mPath.mBoard[0][y] != -1))
    				{
    					distance = mPath.mBoard[0][y];
    					MeilleureSortie = make_pair(0, y);
    				}
    			}
    		break;

    		case (eDown) :
    			for (int x = 0; x < 9; x++)
    			{
    				if ((distance > mPath.mBoard[x][8]) && (mPath.mBoard[x][8] != -1))
    				{
    					distance = mPath.mBoard[x][8];
    					MeilleureSortie = make_pair(x, 8);
    				}
    			}
    		break;
    	}

    	// Maj membres
    	mDistance 			= distance;
    	mMeilleureSortie	= MeilleureSortie;
    	mDistancePond		= mDistance + mId;
    }

    bool IsAlive (void)
    {
    	return (-1 != mX);
    }

    bool IsBlocked (void)
    {
      return (999 == mDistance);
    }
};


struct TWall
{
    typedef vector<TWall> Vector;

    int 	mX;
    int 	mY;
    string 	mOrientation;

    // Constructeur par d�faut
    TWall (void)
	{
    	Raz();
	}

    TWall (int aX, int aY, const string& aOrientation) :
        mX          	(aX),
        mY          	(aY),
		mOrientation	(aOrientation)
    {
    }

    void Raz (void)
    {
        mX				= -1;
        mY				= -1;
        mOrientation.erase();
    }

    bool IsValide (void)
    {
    	return (-1 != mX);
    }

    bool IsCompatible (const Vector& aVector)
    {
    	bool bIsCompatible = true;

    	TWall::Vector::const_iterator iWall;
    	for (iWall = aVector.begin(); iWall != aVector.end(); iWall++)
    	{
    		if (!IsCompatible(*iWall))
    		{
    			bIsCompatible = false;
    			break;
    		}
    	}

    	return bIsCompatible;
    }

    bool IsCompatible (const TWall& aWall)
    {
    	bool bIsCompatible = true;

    	// Les deux sont verticaux, ils ne doivent pas se recouvrir
    	if ((mOrientation == "V") && (aWall.mOrientation == "V") && (mX == aWall.mX))
    	{
    		if ((mY == aWall.mY) || ((mY + 1) == aWall.mY) || (mY == (aWall.mY + 1)))
    		{
    			bIsCompatible = false;
    		}
    	}
    	// Les deux sont horizontaux, ils ne doivent pas se recouvrir
    	else if ((mOrientation == "H") && (aWall.mOrientation == "H") && (mY == aWall.mY))
    	{
    		if ((mX == aWall.mX) || ((mX + 1) == aWall.mX) || (mX == (aWall.mX + 1)))
    		{
    			bIsCompatible = false;
    		}
    	}
    	else if ((mOrientation == "H") && (aWall.mOrientation == "V"))
    	{
    		if ((mX == (aWall.mX - 1)) && (mY == (aWall.mY + 1)))
    		{
    			bIsCompatible = false;
    		}
    	}
    	else if ((mOrientation == "V") && (aWall.mOrientation == "H"))
    	{
    		if ((mX == (aWall.mX + 1)) && (mY == (aWall.mY - 1)))
    		{
    			bIsCompatible = false;
    		}
    	}

    	return bIsCompatible;
    }
};

void DumpPlayers (TPlayer::Vector& aPlayerVector)
{
   cerr << "************* Players ********************" << endl;
   TPlayer::Vector::iterator iPlayer;
   for (iPlayer = aPlayerVector.begin(); iPlayer != aPlayerVector.end(); iPlayer++)
   {
         cerr  << "Player : " << iPlayer->mId << " - X : " << iPlayer->mX << " - Y : " << iPlayer->mY
               << " - WallsLeft : " << iPlayer->mWallsLeft << " - Distance : " << iPlayer->mDistance
               << " - DistancePond : " << iPlayer->mDistancePond << endl;

         if (iPlayer->IsAlive())
         {
            cerr << "************* Path **********************" << endl;
            iPlayer->mPath.Dump();
         }
         else
         {
            cerr << "Dead" << endl;
         }
   }
}

void DumpWalls (TWall::Vector& aWallVector)
{
   cerr << "************* Walls **********************" << endl;
   TWall::Vector::iterator iWall;
   for (iWall = aWallVector.begin(); iWall != aWallVector.end(); iWall++)
   {
         cerr  << "Wall " << " - X : " << iWall->mX << " - Y : " << iWall->mY
               << " - Orientation : " << iWall->mOrientation << endl;
   }
}

void Dump (TPlayer::Vector& aPlayerVector, TWall::Vector& aWallVector)
{
   DumpPlayers (aPlayerVector);
   DumpWalls   (aWallVector);
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

void MoveMyPlayer (TPlayer& aPlayer, TMovesByCell::Matrix& aBoard)
{
	// Initialise le next move
	TCoordonnees 	NextMove = aPlayer.mMeilleureSortie;
	int				distance = aPlayer.mDistance;

	while (distance > 1)
	{
		// Gauche
		if (	(aPlayer.ChercheNextMove (NextMove.first - 1, NextMove.second, distance))
				&& aBoard[NextMove.first][NextMove.second].mbLeft)
		{
			NextMove.first--;
		}
		// Droite
		else if (	(aPlayer.ChercheNextMove (NextMove.first + 1, NextMove.second, distance))
					&& aBoard[NextMove.first][NextMove.second].mbRight)
		{
			NextMove.first++;
		}
		// Haut
		else if (	(aPlayer.ChercheNextMove (NextMove.first, NextMove.second - 1, distance))
					&& aBoard[NextMove.first][NextMove.second].mbUp)
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

	// Compare NextMove � la position courante

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

	cerr 	<< "MoveMyPlayer - MeilleureSortie : (" << aPlayer.mMeilleureSortie.first << "," << aPlayer.mMeilleureSortie.second << ")"
			<< " - NextMove : (" << NextMove.first << "," << NextMove.second << ")" << endl;
}

void TraceTime (const char* apTrace, steady_clock::time_point aStart)
{
    auto DiffTime = (chrono::steady_clock::now() - aStart);
    cerr << apTrace << chrono::duration<double, milli>(DiffTime).count() << endl;
}

void PutWall (const TWall& aWall)
{
	cout << aWall.mX << " " << aWall.mY << " " << aWall.mOrientation << endl;
}

int WhoIsWinning (TPlayer::Vector& aPlayerVector)
{
	int distance = 999;
	int Winner;

	TPlayer::Vector::iterator iPlayer;
	for (iPlayer = aPlayerVector.begin(); iPlayer != aPlayerVector.end(); iPlayer++)
	{
		if ((distance > iPlayer->mDistancePond) && (iPlayer->IsAlive()))
		{
			Winner = iPlayer->mId;
			distance = iPlayer->mDistancePond;
		}
	}

	return Winner;
}

int WhoIsLosing (TPlayer::Vector& aPlayerVector)
{
	int distance = 0;
	int Loser;

	TPlayer::Vector::iterator iPlayer;
	for (iPlayer = aPlayerVector.begin(); iPlayer != aPlayerVector.end(); iPlayer++)
	{
		if ((distance < iPlayer->mDistancePond) && (iPlayer->IsAlive()))
		{
			Loser = iPlayer->mId;
			distance = iPlayer->mDistancePond;
		}
	}

	return Loser;
}

TWall BestWallToBlock (int aWinner, const TPlayer::Vector& aPlayerVector, const TWall::Vector& aWallVector)
{
	TWall BestWall;

	// Initialise à la position de l'adversaire
	BestWall.mX = aPlayerVector[aWinner].mX;
   BestWall.mY = aPlayerVector[aWinner].mY;

	switch (aPlayerVector[aWinner].mDirection)
	{
		case eRight:
			// Cas particulier de la dernière ligne
			if (BestWall.mY == 8)
			{
				BestWall.mY--;
			}
			BestWall.mX++;
			BestWall.mOrientation = "V";
			break;

		case eLeft:
         // Cas particulier de la dernière ligne
         if (BestWall.mY == 8)
         {
            BestWall.mY--;
         }
			BestWall.mOrientation = "V";
			break;

		case eDown:
         // Cas particulier de la dernière colonne
         if (BestWall.mX == 8)
         {
            BestWall.mX--;
         }
			BestWall.mY++;
			BestWall.mOrientation = "H";
			break;
	}

	bool bIsCompatible = BestWall.IsCompatible (aWallVector);
	if (!bIsCompatible)
	{
		BestWall.Raz();
	}

	return BestWall;
}

bool NewWallDontBlock (TWall& aNewWall, TPlayer::Vector& aPlayerVector,
                       TWall::Vector& aWallVector, TMovesByCell::Matrix& aBoard)
{
   bool bDontBlock = true;

   // Ajout du nouveau mur
   aWallVector.push_back(aNewWall);
   DumpWalls(aWallVector);

   MajBoard(aBoard, aWallVector, 9, 9);
   DumpMatrix (aBoard);

   TPlayer::Vector::iterator iPlayer;
   for (iPlayer = aPlayerVector.begin(); iPlayer != aPlayerVector.end(); iPlayer++)
   {
     if (iPlayer->IsAlive())
     {
        iPlayer->ResetPath();
        iPlayer->ComputePath (aBoard);
        iPlayer->ComputeDistance();

        if (iPlayer->IsBlocked())
        {
           cerr  << "NewWallDontBlock - Wall (" << aNewWall.mX << "," << aNewWall.mY << "," << aNewWall.mOrientation << ")"
                 << " is blocking player " << iPlayer->mId << endl;
           bDontBlock = false;
           break;
        }
     }
   }

   DumpPlayers(aPlayerVector);

   return bDontBlock;
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

    TMovesByCell::Matrix		Board;
    steady_clock::time_point 	StartMesure;

    // Initialise le board avec les valeurs initiales
    InitBoard (Board, w, h);

    // game loop
    while (1)
    {
    	// Reset des tableaux
    	PlayerVector.clear();
    	WallVector.clear();

    	// D�marre mesure
    	StartMesure = chrono::steady_clock::now();

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
        TraceTime ("Parcours joueurs : ", StartMesure);

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
        TraceTime ("Parcours Wall : ", StartMesure);

        // Maj du board suite maj du nombre de murs
        MajBoard (Board, WallVector, w, h);
        TraceTime ("MajBoard : ", StartMesure);

        // Calcule les possibilit�s de d�placement
        TPlayer::Vector::iterator iPlayer;
	    for (iPlayer = PlayerVector.begin(); iPlayer != PlayerVector.end(); iPlayer++)
	    {
	    	if (iPlayer->IsAlive())
	    	{
	    		iPlayer->ComputePath (Board);
	    		iPlayer->ComputeDistance();
	    	}
	    }
        TraceTime ("Compute : ", StartMesure);

        // Dump des tableaux
        cerr << "Dump" << endl;
        Dump (PlayerVector, WallVector);
        TraceTime ("Dump - end : ", StartMesure);

        // TODO FAB : A mutualiser
        // D�termine le ranking
        int Winner 	= WhoIsWinning 	(PlayerVector);
        int Loser 	= WhoIsLosing 	(PlayerVector);
        cerr << "Winner : Player " << Winner << " - Loser : Player " << Loser << endl;

        // Je ne suis pas le dernier alors je continue � avancer
        if (myId != Loser)
        {
            // Bouge mon joueur
            MoveMyPlayer (PlayerVector[myId], Board);
            TraceTime ("MoveMyPlayer : ", StartMesure);
        }
        // Je perds donc je vais mettre un mur si c'est possible
        else
        {
        	bool bHaveToMove = false;

        	if (!PlayerVector[myId].mWallsLeft)
        	{
                cerr << "No more wall : have to move" << endl;
                bHaveToMove = true;
        	}
        	else
        	{
        		TWall BestWall = BestWallToBlock (Winner, PlayerVector, WallVector);
				if (BestWall.IsValide())
				{
				   TWall::Vector        WallVectorCopie   = WallVector;
				   TMovesByCell::Matrix BoardCopie        = Board;
				   TPlayer::Vector      PlayerVectorCopie = PlayerVector;

				   if (NewWallDontBlock(BestWall, PlayerVectorCopie, WallVectorCopie, BoardCopie))
				   {
				      PutWall(BestWall);
				   }
				   else
				   {
	                cerr << "Can't put wall bc it is blocking : have to move" << endl;
	                bHaveToMove = true;
				   }
				}
				else
				{
                cerr << "Can't put wall : have to move" << endl;
                bHaveToMove = true;
				}
        	}

        	if (bHaveToMove)
        	{
                // Bouge mon joueur
                MoveMyPlayer (PlayerVector[myId], Board);
                TraceTime ("MoveMyPlayer : ", StartMesure);
        	}
        }
    }
}
