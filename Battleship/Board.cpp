#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>

using namespace std;

class BoardImpl
{
public:
	BoardImpl(const Game& g);
	void clear();
	void block();
	void unblock();
	bool placeShip(Point topOrLeft, int shipId, Direction dir);
	bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
	void display(bool shotsOnly) const;
	bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
	bool allShipsDestroyed() const;

private:
	// TODO:  Decide what private members you need.  Here's one that's likely
	//        to be useful:
	vector<vector<char>> m_grid;  //grid stored in a 2D character vector
	const Game& m_game;
};

BoardImpl::BoardImpl(const Game& g)
	: m_game(g)
{
	vector<char> row(g.cols(), '.');
	vector<vector<char>> grid(g.rows(), row);
	m_grid = grid;
} //done

void BoardImpl::clear()
{
	vector<char> row(m_game.cols(), '.');
	vector<vector<char>> grid(m_game.rows(), row);
	m_grid = grid;
}//done

void BoardImpl::block()
{
	// Block cells with 50% probability
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
			if (randInt(2) == 0)
			{
				m_grid[r][c] = 'X'; // TODO:  Replace this with code to block cell (r,c)   //I use 'X' here because ships cannot have that character
			} //done
}

void BoardImpl::unblock()
{
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (m_grid[r][c] == 'X')
				m_grid[r][c] = '.'; // TODO:  Replace this with code to unblock cell (r,c) if blocked
		} //done
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	//1     check if shipId is valid
	if (shipId >= m_game.nShips() || shipId<0)
		return false;
	//2       check if point is valid
	if (!m_game.isValid(topOrLeft))
		return false;
	if (dir == VERTICAL&&topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
		return false;
	if (dir == HORIZONTAL&&topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
		return false;
	//3 & 4       check if ship would fit in the spot
	if (dir == VERTICAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (m_grid[topOrLeft.r + i][topOrLeft.c] != '.')
				return false;
		}
	}
	if (dir == HORIZONTAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (m_grid[topOrLeft.r][topOrLeft.c + i] != '.')
				return false;
		}
	}
	//5       check if ship already exists on board
	for (int r = 0; r < m_grid.size(); r++)
	{
		for (int c = 0; c < m_grid[r].size(); c++)
		{
			if (m_grid[r][c] == m_game.shipSymbol(shipId))
				return false;
		}
	}

	//all false testing done

	if (dir == VERTICAL)  //place vertical ship
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			m_grid[topOrLeft.r + i][topOrLeft.c] = m_game.shipSymbol(shipId);
		}
	}
	if (dir == HORIZONTAL) //place horizontal ship
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			m_grid[topOrLeft.r][topOrLeft.c + i] = m_game.shipSymbol(shipId);
		}
	}

	return true;
}//done

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	//1    check if shipId is valid
	if (shipId >= m_game.nShips() || shipId<0)
		return false;

	//additional checks if point and position is valid
	if (!m_game.isValid(topOrLeft))
		return false;
	if (dir == VERTICAL&&topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
		return false;
	if (dir == HORIZONTAL&&topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
		return false;

	//2       check if entire ship is at location
	if (dir == VERTICAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (m_grid[topOrLeft.r + i][topOrLeft.c] != m_game.shipSymbol(shipId))
				return false;
		}
	}
	if (dir == HORIZONTAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (m_grid[topOrLeft.r][topOrLeft.c + 1] != m_game.shipSymbol(shipId))
				return false;
		}
	}

	//remove ship
	if (dir == VERTICAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			m_grid[topOrLeft.r + i][topOrLeft.c] = '.';
		}
	}
	if (dir == HORIZONTAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			m_grid[topOrLeft.r][topOrLeft.c + i] = '.';
		}
	}

	return true;
}//done

void BoardImpl::display(bool shotsOnly) const
{
	cout << "  ";
	for (int i = 0; i < m_grid[0].size(); i++)  //print column labels
	{
		cout << i;
	}
	cout << endl;

	for (int i = 0; i < m_grid.size(); i++)
	{
		cout << i << ' '; //print out row labels
		for (int j = 0; j < m_grid[i].size(); j++) //print grid
		{
			char c = m_grid[i][j];
			if (shotsOnly&&c != '.'&&c != 'X'&&c != 'o')
				cout << '.';
			else
				cout << m_grid[i][j];
		}
		cout << endl;
	}
} //done

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId) //CHECK THIS THOROUGHLY
{
	if (!m_game.isValid(p))  //checks if point is in board
		return false;

	char c = m_grid[p.r][p.c];

	if (c == 'X' || c == 'o') //checks if point already attacked
		return false;

	if (c == '.') //missed shot
	{
		m_grid[p.r][p.c] = 'o';
		shotHit = false;
		return true;
	}

	shotHit = true;
	m_grid[p.r][p.c] = 'X';


	//determine shipId
	int id;
	for (id = 0; id < m_game.nShips(); id++)
	{
		if (c == m_game.shipSymbol(id))
			break;
	}
	if (id >= m_game.nShips()) //make sure no bad access occurs
		return false;


	for (int i = 0; i < m_grid.size(); i++) //checks if ship destroyed
	{
		for (int j = 0; j < m_grid[i].size(); j++)
		{
			if (c == m_grid[i][j])
			{
				shipDestroyed = false;
				return true;
			}
		}
	}

	shipDestroyed = true;
	shipId = id;

	return true;
}

bool BoardImpl::allShipsDestroyed() const
{
	for (int i = 0; i < m_grid.size(); i++)
	{
		for (int j = 0; j < m_grid[i].size(); j++)
		{
			if (m_grid[i][j] != 'X'&&m_grid[i][j] != 'o'&&m_grid[i][j] != '.')
				return false;
		}
	}
	return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
	m_impl = new BoardImpl(g);
}

Board::~Board()
{
	delete m_impl;
}

void Board::clear()
{
	m_impl->clear();
}

void Board::block()
{
	return m_impl->block();
}

void Board::unblock()
{
	return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
	m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
	return m_impl->allShipsDestroyed();
}
