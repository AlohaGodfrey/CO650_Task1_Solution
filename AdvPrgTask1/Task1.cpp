#include <iostream>//used to display output to console
#include <string> //used specifically to convert coords to string data
#include <windows.h>//used to obtain retrieve and manipulate system time data

using namespace std;

static int nextID = 0;//unsure how to implement instances, so deployed an alternative idnum() method
typedef void (*FP)(); //typedef for void funtion with no parameters
void initMsg(); //  declare standalone functions...
void termMsg(); // prototype for terminate
void studentInfo();// prototype for display Student

class GameComponent
{
private:
	int id; //protected to allow access for derived/child classes
public:
	GameComponent(void);//constructor
	~GameComponent(void);//destructor
	virtual void Update(SYSTEMTIME& st); //virtual function is needed so compiler...
	static int idnum();
	int getID() const;//accesor for private id used in dervid class
};

GameComponent::GameComponent(void)
{
	//no need for a setter member function as id is only assigned within class scope
	id = idnum();
}

int GameComponent::getID() const { 
	//getter member function
	return id;
}

int GameComponent::idnum() 
{
	nextID = nextID + 1;
	return nextID;
}


GameComponent::~GameComponent(void)
{
}

void GameComponent::Update(SYSTEMTIME& st)
{
	//prints out ID along with formatted system time to console
	cout << "ID: " << getID() << " Updated @ ";
	printf("%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
}

class DrawableGameComponent :
	public GameComponent
{
private:
	int x;
	int y;
	void ChangeDirection();
	void Draw();
	string coordString;
	int rnum; //used for random number
public:
	//uses strong enum (scoped) instead of standard enum(unscoped)
	//'enum class' allows for specifying of underlying type
	enum class Direction { Left, Right, Up, Down };
	const int SCREEN_HEIGHT = 80;
	const int SCREEN_WIDTH = 20;
	Direction direction;
	DrawableGameComponent();//constructor
	DrawableGameComponent(int x, int y);
	~DrawableGameComponent();//destructor
	void Update (SYSTEMTIME& st);
};

DrawableGameComponent::DrawableGameComponent()
{
	x = 0;
	y = 0;
	direction = Direction::Right;
	rnum = 0;

}

DrawableGameComponent::DrawableGameComponent(int x, int y):x(x),y(y) //sets the coord (x,y) properties to the corresponding arguments
{
	direction = Direction::Right;
	rnum = 0;
}

DrawableGameComponent::~DrawableGameComponent()
{
}


void DrawableGameComponent::Update(SYSTEMTIME& st)
{
	cout << "ID: " << getID() << " Updated @ ";
	printf("%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond); 
	//Increments and decrements (x,y) coords dependant on direction
	switch (direction) {
	case Direction::Right:
		//clamp x value so that max value is SCREEN_WIDTH const
		x = ((x + 1) > SCREEN_WIDTH) ? x : x + 1;;
		//starts building string used in Draw() member function
		coordString = "Right";
		break;
	case Direction::Left:
		//clamp x value so that min value is 0
		x = ((x - 1) < 0) ? x : x - 1;;
		coordString = "Left";
		break;
	case Direction::Up:
		//clamp y value so that max value is SCREEN_HEIGHT const
		y = ((y + 1) > SCREEN_HEIGHT) ? y : y + 1;;
		coordString = "Up";
		break;
	case Direction::Down:
		//clamp y value so that min value is 0
		y = ((y - 1) < 0) ? y : y - 1;;
		coordString = "Down";
		break;
	default:;
	}

	Draw();
	ChangeDirection();
	
}

void DrawableGameComponent::Draw()
{
	//builds string that displays current direction along (with x,y) coords
	coordString.append(" : X:").append(std::to_string(x));
	coordString.append(" Y:").append(std::to_string(y));
	std::cout << coordString << std::endl;
}

void DrawableGameComponent::ChangeDirection()
{

	//Get system time for random number generator
	srand(time(NULL));

	//generates a random number between 1-3
	int rnum = (rand() %3)+1;

	//nested select case statement used with 
	//random number generator to determine new direction
	//statement prevents previous direction to be selected.
	switch (direction) {
	case Direction::Right:
		switch (rnum) {
		case 1: direction = Direction::Left; break;
		case 2: direction = Direction::Up; break;
		case 3: direction = Direction::Down; break;
		default:;
		}
		break;
	case Direction::Left:
		switch (rnum) {
		case 1: direction = Direction::Right; break;
		case 2: direction = Direction::Up; break;
		case 3: direction = Direction::Down; break;
		default:;
		}
		break;
	case Direction::Up:
		switch (rnum) {
		case 1: direction = Direction::Left; break;
		case 2: direction = Direction::Right; break;
		case 3: direction = Direction::Down; break;
		default:;
		}
		break;
	case Direction::Down:
		switch (rnum) {
		case 1: direction = Direction::Left; break;
		case 2: direction = Direction::Up; break;
		case 3: direction = Direction::Right; break;
		default:;
		}
		break;
	default:;
	}
}



class Game
{
private:
	int componentCount;
	const int TICKS_1000MS = 1000;
	GameComponent** components;
	FP initialise;
	FP terminate;
	FP student;
	SYSTEMTIME st;
public:
	Game(int maxComponents);
	void Run();
	void SetInitialise(FP init); // changed the signature to take a function 
	void SetTerminate(FP term);
	void ShowStudent(FP dStd);
	void Add(GameComponent* nComponent);
};

Game::Game(int maxComponents)
{
	terminate = 0;
	initialise = 0;
	student = 0;
	components = new GameComponent * [maxComponents];
	componentCount = 0;
	GetSystemTime(&st);
}

void Game::Run()
{
	ShowStudent(&studentInfo);//pass the address of the free student function
	student();

	SetInitialise(&initMsg); // pass the address of the free initialise function
	initialise();

	//for loop designed to loop five times
	for (int n = 0; n < 5; n++) 
	{
		//updates system time variable
		GetSystemTime(&st);
		//nested for loops for each item in components array
		for (int i = 0; i < componentCount; i++)
		{
			components[i]->Update(st);
		}
		//system will wait 1 second 
		Sleep(TICKS_1000MS);
	}

	SetTerminate(&termMsg); // pass the address of the free terminate function
	terminate();
}

void Game::ShowStudent(FP dStd)
{
	this->student = dStd; //  uses function pointer
}

void Game::SetInitialise(FP init) //  uses function pointer
{
	this->initialise = init;
}

void Game::SetTerminate(FP term) // uses function pointer
{
	this->terminate = term;
}

void Game::Add(GameComponent* nComponent)
{
	components[componentCount++] = nComponent; //adds gamecomponent.. similarly to wizards week6 exercise
}

void studentInfo()
{
	cout << "//Student Name: Immanuel Godfrey" << endl;
	cout << "//Student ID: 21817368" << endl;
	cout << "//Course ID: CO650 - Advanced Programming" << endl;
	cout << endl;
}

void initMsg() {
	cout << "Initialising game" << endl; // display required text
}

void termMsg() {
	cout << "Terminating game" << endl; // display required text
}

int main()
{
	//creates new dynamic game object
	Game* myGame;
	myGame = new Game(3);

	//creates new dynamic GameComponent object
	GameComponent* myGameComponent;
	myGameComponent = new GameComponent();

	//creates new dynamic DrawableGameComponent object
	DrawableGameComponent* myDGC;
	myDGC = new DrawableGameComponent(2, 2);
	
	myGame->Add(myGameComponent);
	myGame->Add(myDGC);
	myGame->Run();

	

}

