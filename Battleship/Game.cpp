#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

class GameImpl
{
public:
	GameImpl(int nRows, int nCols);
	int rows() const;
	int cols() const;
	bool isValid(Point p) const;
	Point randomPoint() const;
	bool addShip(int length, char symbol, string name);
	int nShips() const;
	int shipLength(int shipId) const;
	char shipSymbol(int shipId) const;
	string shipName(int shipId) const;
	Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:
	int m_r;
	int m_c;
	struct Ship  //struct to store all values of each ship
	{
		int m_len;
		char m_sym;
		string m_name;
	};
	vector<Ship> m_ships;  //vector to keep track of all ships
};

void waitForEnter()
{
	cout << "Press enter to continue: ";
	cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : m_r(nRows), m_c(nCols)
{} //done

int GameImpl::rows() const
{
	return m_r;
}//done

int GameImpl::cols() const
{
	return m_c;
}//done

bool GameImpl::isValid(Point p) const
{
	return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
	return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	Ship s;                  //create new ship with given properties
	s.m_len = length;
	s.m_sym = symbol;
	s.m_name = name;
	m_ships.push_back(s);    //add ship to m_ships vector
	return true;  //should always work
}

int GameImpl::nShips() const
{
	return m_ships.size();
} //done

int GameImpl::shipLength(int shipId) const
{
	return m_ships[shipId].m_len;
} //done

char GameImpl::shipSymbol(int shipId) const
{
	return m_ships[shipId].m_sym;
} //done

string GameImpl::shipName(int shipId) const
{
	return m_ships[shipId].m_name;
} //done

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
	if (!p1->placeShips(b1))  //if either player is unable to place ships, return nullptr
		return nullptr;
	if (!p2->placeShips(b2))
		return nullptr;

	while ((!b1.allShipsDestroyed()) && (!b2.allShipsDestroyed())) //runs until either player wins, however, there are more checks for winner in this loop, so this should never be true
	{
		//p1 turn
		cout << p1->name() << "'s turn.  Board for " << p2->name() << ":" << endl;
		b2.display(p1->isHuman());

		bool shotHit = false;
		bool shipDestroyed = false;
		int shipId = -1;

		Point p = p1->recommendAttack();
		bool validShot = b2.attack(p, shotHit, shipDestroyed, shipId);
		p1->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
		p2->recordAttackByOpponent(p);

		if (validShot) //all outputs copy the given sample program
		{
			if (shotHit)
			{
				if (shipDestroyed)
					cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and destroyed the " << shipName(shipId) << ", resulting in:" << endl;
				else
					cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and hit something, resulting in:" << endl;
			}
			else
				cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and missed, resulting in:" << endl;
			b2.display(p1->isHuman());
		}
		else
		{
			cout << p1->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
		}

		if (b2.allShipsDestroyed()) //checks if p1 just won after this turn
		{
			cout << p1->name() << " wins!" << endl;
			if (p2->isHuman())  //if loser is human, display opponent's board
			{
				cout << "Here's where " << p1->name() << "'s ships were:" << endl;
				b1.display(false);
			}
			return p1;
		}

		if (shouldPause)
			waitForEnter();

		//p2 turn (same as p1 turn, but for p2)
		cout << p2->name() << "'s turn.  Board for " << p1->name() << ":" << endl;
		b1.display(p2->isHuman());

		p = p2->recommendAttack();

		validShot = b1.attack(p, shotHit, shipDestroyed, shipId);
		p2->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
		p1->recordAttackByOpponent(p);

		if (validShot)
		{
			if (shotHit)
			{
				if (shipDestroyed)
					cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and destroyed the " << shipName(shipId) << ", resulting in:" << endl;
				else
					cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and hit something, resulting in:" << endl;
			}
			else
				cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and missed, resulting in:" << endl;
			b1.display(p2->isHuman());
		}
		else
		{
			cout << p2->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
		}

		if (b1.allShipsDestroyed())
		{
			cout << p2->name() << " wins!" << endl;
			if (p1->isHuman())
			{
				cout << "Here's where " << p2->name() << "'s ships were:" << endl;
				b2.display(false);
			}
			return p2;
		}

		if (shouldPause)
			waitForEnter();
	}

	return nullptr; //should never run
} //TODO: CHECK THIS A LOT, IDK WHAT THE FUNCTIONS ACTUALLY DO AT THIS POINT

  //******************** Game functions *******************************

  // These functions for the most part simply delegate to GameImpl's functions.
  // You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
	if (nRows < 1 || nRows > MAXROWS)
	{
		cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
		exit(1);
	}
	if (nCols < 1 || nCols > MAXCOLS)
	{
		cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
		exit(1);
	}
	m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
	delete m_impl;
}

int Game::rows() const
{
	return m_impl->rows();
}

int Game::cols() const
{
	return m_impl->cols();
}

bool Game::isValid(Point p) const
{
	return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
	return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
	if (length < 1)
	{
		cout << "Bad ship length " << length << "; it must be >= 1" << endl;
		return false;
	}
	if (length > rows() && length > cols())
	{
		cout << "Bad ship length " << length << "; it won't fit on the board"
			<< endl;
		return false;
	}
	if (!isascii(symbol) || !isprint(symbol))
	{
		cout << "Unprintable character with decimal value " << symbol
			<< " must not be used as a ship symbol" << endl;
		return false;
	}
	if (symbol == 'X' || symbol == '.' || symbol == 'o')
	{
		cout << "Character " << symbol << " must not be used as a ship symbol"
			<< endl;
		return false;
	}
	int totalOfLengths = 0;
	for (int s = 0; s < nShips(); s++)
	{
		totalOfLengths += shipLength(s);
		if (shipSymbol(s) == symbol)
		{
			cout << "Ship symbol " << symbol
				<< " must not be used for more than one ship" << endl;
			return false;
		}
	}
	if (totalOfLengths + length > rows() * cols())
	{
		cout << "Board is too small to fit all ships" << endl;
		return false;
	}
	return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
	return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
	if (p1 == nullptr || p2 == nullptr || nShips() == 0)
		return nullptr;
	Board b1(*this);
	Board b2(*this);
	return m_impl->play(p1, p2, b1, b2, shouldPause);
}

