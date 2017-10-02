#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
	AwfulPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
	: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
	// Clustering ships is bad strategy
	for (int k = 0; k < game().nShips(); k++)
		if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
			return false;
	return true;
}

Point AwfulPlayer::recommendAttack()
{
	if (m_lastCellAttacked.c > 0)
		m_lastCellAttacked.c--;
	else
	{
		m_lastCellAttacked.c = game().cols() - 1;
		if (m_lastCellAttacked.r > 0)
			m_lastCellAttacked.r--;
		else
			m_lastCellAttacked.r = game().rows() - 1;
	}
	return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
	bool result(cin >> r >> c);
	if (!result)
		cin.clear();  // clear error state so can do more input operations
	cin.ignore(10000, '\n');
	return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//done
class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual bool isHuman() const;
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {}
	virtual void recordAttackByOpponent(Point p) {}
private:
	bool shipCantFit(Board& b, Direction d, int shipId);
};
HumanPlayer::HumanPlayer(string nm, const Game& g) : Player(nm, g)
{}
bool HumanPlayer::isHuman() const
{
	return true;
}
bool HumanPlayer::placeShips(Board& b)
{
	cout << name() << " must place " << game().nShips() << " ships." << endl;

	for (int i = 0; i < game().nShips(); i++)
	{
		if (shipCantFit(b, HORIZONTAL, i) && shipCantFit(b, VERTICAL, i))
			return false;

		char dir = '\0';
		b.display(false);
		Direction d;
		while (!(dir == 'h' || dir == 'v')) //get direction from user input
		{
			cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
			cin >> dir;
			cin.ignore(10000, '\n');
			if (dir == 'h')
				d = HORIZONTAL;
			else if (dir == 'v')
				d = VERTICAL;
			else
				cout << "Direction must be h or v." << endl;
		}

		if (shipCantFit(b, d, i)) //checks to see if ship can fit after direction is chosen
			return false;

		Point p(-1, -1);
		while (!b.placeShip(p, i, d))
		{
			if (dir == 'h')
				cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
			else
				cout << "Enter row and column of topmost cell (e.g. 3 5): ";
			cin >> p.r;
			cin >> p.c;
			if (cin.fail())
			{
				cin.clear();
				cin.ignore(10000, '\n');
				cout << "You must enter two integers." << endl;
				continue;
			}
			cin.ignore(10000, '\n');
			if (b.placeShip(p, i, d))
				break;
			else
				cout << "The ship can not be placed there." << endl;
		}
	}
	return true;
}

bool HumanPlayer::shipCantFit(Board& b, Direction d, int shipId) //helper function that checks if a certain ship can fit anywhere on the board
{
	for (int r = 0; r < game().rows(); r++)
	{
		for (int c = 0; c < game().cols(); c++)
		{
			if (b.placeShip(Point(r, c), shipId, d))
			{
				b.unplaceShip(Point(r, c), shipId, d);
				return false;
			}
		}
	}
	return true;
}

Point HumanPlayer::recommendAttack()
{
	for (;;) //infinite loop to keep asking for input until valid input is given
	{
		Point p;
		cout << "Enter the row and column to attack (e.g, 3 5): ";
		cin >> p.r;
		cin >> p.c;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "You must enter two integers." << endl;
			continue;
		}
		cin.ignore(10000, '\n');
		return p;
	}
	return Point(-1, -1); //never runs
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p) {}
private:
	bool shipRec(Board& b, int shipId);
	int state;
	vector<Point> attacks;
	Point transition;

};
MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g), state(1)
{}
bool MediocrePlayer::placeShips(Board& b)
{
	for (int i = 0; i < 50; i++)  //tries to placeShips 50 times
	{
		b.block();
		if (shipRec(b, 0))
		{
			b.unblock();
			return true;
		}
		b.unblock();
	}
	return false;
}//done
bool MediocrePlayer::shipRec(Board& b, int shipId) //recursion that goes through each ship
{
	if (shipId >= game().nShips())
		return true;

	for (int r = 0; r < game().rows(); r++) //tries to place ship in evrey spot on the board
	{
		for (int c = 0; c < game().cols(); c++)
		{
			if (b.placeShip(Point(r, c), shipId, VERTICAL))
				if (shipRec(b, shipId + 1))
					return true;
				else
					b.unplaceShip(Point(r, c), shipId, VERTICAL);
			if (b.placeShip(Point(r, c), shipId, HORIZONTAL))
				if (shipRec(b, shipId + 1))
					return true;
				else
					b.unplaceShip(Point(r, c), shipId, HORIZONTAL);
		}
	}
	return false;
}

// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

Point MediocrePlayer::recommendAttack()
{
	if (state == 2) //in state 2, algorithm finishes off a ship
	{
		vector<Point> temp; //stores all points that could be attacked
		for (int i = transition.r - 4; i <= transition.r + 4; i++) //add points up and down of transition point
		{
			Point p(i, transition.c);
			if (game().isValid(p))
			{
				bool shouldPush = true;
				for (int j = 0; j < attacks.size(); j++)
				{
					if (p.r == attacks[j].r && p.c == attacks[j].c)
					{
						shouldPush = false;
						break;
					}
				}
				if (shouldPush)
					temp.push_back(p);
			}
		}
		for (int i = transition.c - 4; i <= transition.c + 4; i++) //add points left and right of transition point
		{
			Point p(transition.r, i);
			if (game().isValid(p))
			{
				bool shouldPush = true;
				for (int j = 0; j < attacks.size(); j++)
				{
					if (p.r == attacks[j].r && p.c == attacks[j].c)
					{
						shouldPush = false;
						break;
					}
				}
				if (shouldPush)
					temp.push_back(p);
			}
		}
		if (temp.empty()) //if there are no more positions in cross to attack, do state 1 algorithm below
		{
			state = 1;
		}
		else
		{
			return temp[randInt(temp.size())]; //choose random point from temp
		}
	}
	//state 1 algorithm
	Point p = game().randomPoint(); //choose random point on grid that has not been attacked
	for (int i = 0; i < attacks.size(); i++)
	{
		if (p.r == attacks[i].r && p.c == attacks[i].c)
		{
			p = game().randomPoint();
			i = -1;
		}
	}
	return p;
}
void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	if (validShot)
	{
		attacks.push_back(p); //keep track of all attacks
		if (shotHit)
		{
			if (shipDestroyed)
			{
				if (state == 2)
					state = 1; //if ship was destroyed in state 2, switch back to state 1
			}
			else
			{
				if (state == 1) //if a ship is found, switch to state 2 to target that ship
				{
					transition = p;
					state = 2;
				}
			}
		}
	}
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
	GoodPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p) {}
private:
	bool isAttacked(Point p);
	bool shipRec(Board& b, int shipId);
	vector<Point> attacks;
	Board m_b; //board keeps track of attacked positions and helps calculate probabilities
	vector<bool> m_ships; //false if ship is destroyed, true if not
	int state;
	queue<Point> transition; //transition points (like in MediocrePlayer) but keeps track of all potential transitions
	int dir; //determines direction of target attack 0=N, 1=E, 2=S, 3=W, -1=none
};

bool GoodPlayer::isAttacked(Point p)  //helper function that sees if a point has been attacked before
{
	for (int i = 0; i < attacks.size(); i++)
	{
		if (p.r == attacks[i].r&&p.c == attacks[i].c)
			return true;
	}
	return false;
}

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g), m_b(g), state(1), dir(-1)
{
	vector<bool> temp(g.nShips(), true);
	m_ships = temp;
}

bool GoodPlayer::placeShips(Board& b) //placeShips is same as MediocrePlayer (improvements not needed to reach 80% win rate)
{
	for (int i = 0; i < 50; i++)
	{
		b.block();
		if (shipRec(b, 0))
		{
			b.unblock();
			return true;
		}
		b.unblock();
	}
	return false;
}

bool GoodPlayer::shipRec(Board& b, int shipId)
{
	if (shipId >= game().nShips())
		return true;

	for (int r = 0; r < game().rows(); r++)
	{
		for (int c = 0; c < game().cols(); c++)
		{
			if (b.placeShip(Point(r, c), shipId, VERTICAL))
				if (shipRec(b, shipId + 1))
					return true;
				else
					b.unplaceShip(Point(r, c), shipId, VERTICAL);
			if (b.placeShip(Point(r, c), shipId, HORIZONTAL))
				if (shipRec(b, shipId + 1))
					return true;
				else
					b.unplaceShip(Point(r, c), shipId, HORIZONTAL);
		}
	}
	return false;
}

Point GoodPlayer::recommendAttack()
{
	if (state == 2) //state 2 is an enhanced "target" algorithm that more efficiently destroys a ship
	{
		while (!transition.empty())
		{
			if (dir == 0) //North
			{
				for (int i = 2; i < game().rows(); i++)
				{
					if (game().isValid(Point(transition.front().r - i, transition.front().c)) && !isAttacked(Point(transition.front().r - i, transition.front().c)))
						return Point(transition.front().r - i, transition.front().c);
				}
				dir = -1;
			}
			else if (dir == 1) //east
			{
				for (int i = 2; i < game().cols(); i++)
				{
					if (game().isValid(Point(transition.front().r, transition.front().c + i)) && !isAttacked(Point(transition.front().r, transition.front().c + i)))
						return Point(transition.front().r, transition.front().c + i);
				}
				dir = -1;
			}
			else if (dir == 2) //south
			{
				for (int i = 2; i < game().rows(); i++)
				{
					if (game().isValid(Point(transition.front().r + i, transition.front().c)) && !isAttacked(Point(transition.front().r + i, transition.front().c)))
						return Point(transition.front().r + i, transition.front().c);
				}
				dir = -1;
			}
			else if (dir == 3) //west
			{
				for (int i = 2; i < game().cols(); i++)
				{
					if (game().isValid(Point(transition.front().r, transition.front().c - i)) && !isAttacked(Point(transition.front().r, transition.front().c - i)))
						return Point(transition.front().r, transition.front().c - i);
				}
				dir = -1;
			}
			if (dir == -1) //if direction not yet determined
			{
				if (game().isValid(Point(transition.front().r - 1, transition.front().c)) && !isAttacked(Point(transition.front().r - 1, transition.front().c)))
					return Point(transition.front().r - 1, transition.front().c);
				if (game().isValid(Point(transition.front().r, transition.front().c + 1)) && !isAttacked(Point(transition.front().r, transition.front().c + 1)))
					return Point(transition.front().r, transition.front().c + 1);
				if (game().isValid(Point(transition.front().r + 1, transition.front().c)) && !isAttacked(Point(transition.front().r + 1, transition.front().c)))
					return Point(transition.front().r + 1, transition.front().c);
				if (game().isValid(Point(transition.front().r, transition.front().c - 1)) && !isAttacked(Point(transition.front().r, transition.front().c - 1)))
					return Point(transition.front().r, transition.front().c - 1);
			}
			transition.pop();
		}
	}//state 1 estimates the probability that a ship will be at a certain Point and chooses most likely point
	vector<int> row(game().cols(), 1);
	vector<vector<int>> probabilities(game().rows(), row); //probability calculated by adding each permutation in which a ship can be at each point, then multiplying permutations for all ships
	for (int i = 0; i < game().nShips(); i++)
	{
		if (m_ships[i]) //only looks at ships that aren't destroyed
		{
			vector<int> temp(game().cols(), 0);
			vector<vector<int>> prob(game().rows(), temp);
			for (int r = 0; r < game().rows(); r++)
			{
				for (int c = 0; c < game().cols(); c++)
				{
					if (m_b.placeShip(Point(r, c), i, VERTICAL)) //places and unplaces ships to add up all possible configurations of ship
					{
						for (int j = 0; j < game().shipLength(i); j++)
						{
							prob[r + j][c]++;
						}
						m_b.unplaceShip(Point(r, c), i, VERTICAL);
					}
					if (m_b.placeShip(Point(r, c), i, HORIZONTAL))
					{
						for (int j = 0; j < game().shipLength(i); j++)
						{
							prob[r][c + j]++;
						}
						m_b.unplaceShip(Point(r, c), i, HORIZONTAL);
					}
				}
			}
			for (int r = 0; r < probabilities.size(); r++)
			{
				for (int c = 0; c < probabilities[r].size(); c++)
				{
					probabilities[r][c] *= prob[r][c];
				}
			}
		}
	}
	int maxProb = 0; //find the maximum probability among all Points
	for (int r = 0; r < probabilities.size(); r++)
	{
		for (int c = 0; c < probabilities[r].size(); c++)
		{
			if (probabilities[r][c] > maxProb)
				maxProb = probabilities[r][c];
		}
	}
	vector<Point> targets;
	for (int r = 0; r < probabilities.size(); r++)
	{
		for (int c = 0; c < probabilities[r].size(); c++)
		{
			if (probabilities[r][c] == maxProb)
				targets.push_back(Point(r, c));
		}
	}

	return targets[randInt(targets.size())]; //choose random Point among all maximum probability points
}
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	bool a;
	bool b;
	int c;
	m_b.attack(p, a, b, c); //attacks recorded on board and vector
	attacks.push_back(p);
	if (validShot)
	{
		if (shotHit)
		{
			if (shipDestroyed)
			{
				m_ships[shipId] = false;
				dir = -1;
				if (transition.empty())
					state = 1;
				else
					transition.pop(); //look at next transition point
			}
			else
			{
				transition.push(p);
				if (state == 2 && dir == -1) //determine direction
				{
					if (p.r == transition.front().r - 1)
						dir = 0;
					else if (p.c == transition.front().c + 1)
						dir = 1;
					else if (p.r == transition.front().r + 1)
						dir = 2;
					else if (p.c == transition.front().c - 1)
						dir = 3;
				}
				if (state == 1)
				{
					state = 2;
				}
			}
		}
		else
		{
			if (state == 2)
				dir = -1;  //previously chosen direction is incorrect, determine direction again
		}
	}
}









//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
	static string types[] = {
		"human", "awful", "mediocre", "good"
	};

	int pos;
	for (pos = 0; pos != sizeof(types) / sizeof(types[0]) &&
		type != types[pos]; pos++)
		;
	switch (pos)
	{
	case 0:  return new HumanPlayer(nm, g);
	case 1:  return new AwfulPlayer(nm, g);
	case 2:  return new MediocrePlayer(nm, g);
	case 3:  return new GoodPlayer(nm, g);
	default: return nullptr;
	}
}