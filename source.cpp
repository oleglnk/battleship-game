//ctime используется для функции time
#include <ctime>
#include <iostream>
using namespace std;
//Размер имени
const int NameSize = 30;
//Структура для хранения информации про игрока
//name - имя игрока
//score - счет
struct Player
{
	char name[NameSize];
	int score;
};
/*
*	Cell - перечисление для типов ячеек
*	EMPTY - пустая ячейка
*	MISS - ячейка с промахом
*	BLOCK - ячейка с блоком корабля
*	BLOCK - ячейка с уничтоженным блоком корабля
*/
enum Cell
{
	EMPTY = 0,
	MISS,
	BLOCK,
	DESTROYED
};
/*
*	Размеры поля
*	FieldWidth - ширина поля
*	FieldHeight - высота поля
*/
const int FieldWidth = 10;
const int FieldHeight = 10;
/*
*	Структура хранящая игровое состояние
*	playerField - поле игрока
*	aiField - поле ИИ
*	player1AvailableShips - доступные корабли (точнее их блоки) игрока
*	aiAvailableShips - доступные корабли (точнее их блоки) ИИ
*	player1 - данные игрока
*	ai - данные ИИ
*/
struct GameState
{
	Cell playerField[FieldHeight][FieldWidth];
	Cell aiField[FieldHeight][FieldWidth];
	int player1AvailableShips;
	int aiAvailableShips;
	Player player1;
	Player ai;
};
//Функция для расставления кораблей на поле
//autoAlloc: true - автоматическое расставление, false - ручное
void ShipsAlloc(Cell field[][FieldWidth], bool autoAlloc);
//Вывод игры на экран
void DisplayGame(const GameState &game);
/*
*	Функция хода игрока
*	field - поле противника
*	AvailableShips - доступные корабли противника
*	player - игрок, который делает ход (которому засчитываются очки)
*	ai - является ли игрок ИИ
*/
int PlayerMove(Cell field[][FieldWidth], int &AvailableShips, Player &player, bool ai);
//Загрузка игры game из файла с именем filename
int LoadGame(const char *filename, GameState &game);
//Сохранение игры game в файла с именем filename
void SaveGame(const char *filename, const GameState &game);
//Сохранение счета игрока
void SavePlayerScore(const Player &newScore);
//Загрузка очков игроков
void LoadPlayersScores(Player *&players, int &recordsCount);
//Вывод очков игроков на экран
void PrintPlayerScore(const Player *players, int recordsCount);

void main()
{
	//Переменная хранящая, игровое состояние
	GameState game;
	//Инициализация генератора случайных чисел
	srand(time(NULL));
	for (int choice = 0;;)
	{
		system("cls");
		//Вывод меню
		for (;;)
		{
			cout << "Sea battle" << endl;
			cout << "1. Play" << endl;
			cout << "2. Load game" << endl;
			cout << "3. Game score" << endl;
			cout << "4. Exit" << endl;
			cin >> choice;
			if (choice < 1 || choice > 4)
				cout << "Invalid input. Try again!" << endl;
			else
				break;
		}
		if (choice == 4)
			break;
		system("cls");
		switch (choice)
		{
		case 1:
		case 2:
		{
			//Если выбрана новая игра
			if (choice == 1)
			{
				//Вводим имя игрока
				cout << "Please enter your name: "; cin >> game.player1.name;
				//Обнуляем все ячейки на полях
				for (int i = 0; i < FieldHeight; i++)
					for (int j = 0; j < FieldWidth; j++)
					{
						game.playerField[i][j] = EMPTY;
						game.aiField[i][j] = EMPTY;
					}
				//Даем имя ИИ
				strcpy(game.ai.name, "AI");
				//Ставим начальные условия:
				//Количество доступных блоков кораблей - 20:
				//4*однопалубных + 3*двухпалубных +... = 20
				//Счет у всех равен 0
				game.ai.score = 0;
				game.player1.score = 0;
				game.aiAvailableShips = 20;
				game.player1AvailableShips = 20;
				//Предлагаем вариант размещения кораблей: вручную или автоматически
				for (;;)
				{
					cout << "Select type of ship's accommodation" << endl;
					cout << "1. Manual" << endl;
					cout << "2. Automatic" << endl;
					cin >> choice;
					if (choice < 1 || choice > 2)
						cout << "Invalid input. Try again!" << endl;
					else
						break;
				}
				if (choice == 1)
					ShipsAlloc(game.playerField, false);
				else
					//Расставляем корабли автоматически для игрока-человека
					ShipsAlloc(game.playerField, true);
				//Расставляем корабли автоматически для игрока-ИИ
				ShipsAlloc(game.aiField, true);
			}
			else
			{
				//Загружаем готовую игру
				char saveFName[30];
				//Вводим название файла с сохранением
				cout << "Enter file name with saved game: "; cin >> saveFName;
				//Загружаем его
				if (!LoadGame(saveFName, game))
				{
					cout << "Can't load file" << endl;
					system("pause");
					break;
				}
			}
			//Условие окночания игры, если значение равно нулю, то продолжаем игру
			int condition = 0;

			//Игровой цикл
			for (int step = 1; step <= 100; step++)
			{
				//Выводим игру на экран
				DisplayGame(game);
				//Ход игрока
				condition = PlayerMove(game.aiField, game.aiAvailableShips, game.player1, false);
				//Если игрок решил выйти, то condition не равно нулю, значит выходим
				if (condition != 0)
					break;
				//Если количество доступных кораблей у ИИ равно 0
				//То условие окончание игры = 1, выходим из игрового цикла
				if (game.aiAvailableShips == 0)
				{
					condition = 1;
					break;
				}
				else
				{
					//Ход ИИ
					PlayerMove(game.playerField, game.player1AvailableShips, game.ai, true);
					//Если количество доступных кораблей у игрока равно 0
					//То условие окончание игры = 2, выходим из игрового цикла
					if (game.player1AvailableShips == 0)
					{
						condition = 2;
						break;
					}
				}
			}
			//Проверяем условие окончания игры
			/*
			*	-2 - игра была остановлена пренудительно
			*	-1 - игра была сохранена
			*	1 - выиграл игрок
			*	2 - выиграл ИИ
			*/
			switch (condition)
			{
			case -2:
			{
				cout << "Game stopped." << endl;
				cout << "Your progrss won't be saved" << endl;
				system("pause");
			}
			break;
			case -1:
			{
				char filename[30];
				cout << "Saving." << endl;
				cout << "Enter file name for game's save: "; cin >> filename;
				SaveGame(filename, game);
				system("pause");
			}
			break;
			case 1:
			{
				cout << "Congradulations!" << endl;
				cout << "You Win!" << endl;
				SavePlayerScore(game.player1);
				system("pause");
			}
			break;
			case 2:
			{
				cout << "Oh no! AI wins!" << endl;
				cout << "You lose!" << endl;
				SavePlayerScore(game.ai);
				system("pause");
			}
			break;
			}
		}
		break;
		case 3:
		{
			//players - список игроков и их рекордов
			Player *players = nullptr;
			//playersCount - количество игроков
			int playersCount;
			//Считываем данные из файла
			LoadPlayersScores(players, playersCount);
			if (players != nullptr)
			{
				cout << "Score table" << endl;
				cout << "#  Name  Score" << endl;
				//Выводим данные на экран
				PrintPlayerScore(players, playersCount);
			}
			else
				cout << "No scores! =(" << endl;
			system("pause");
			//Осовобождаем память
			delete[]players;
		}
		break;
		}
	}
}

void ShipsAlloc(Cell field[][FieldWidth], bool autoAlloc)
{
	int i, j;
	//Количество кораблей
	const int ship_count = 10;
	//Корабли по размерам
	int ships[ship_count] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
	//Расположение:
	//0 - горизонтальное
	//1 - вертикальное
	int orientation;
	//Массив занятых ячеек
	int occupied[FieldHeight][FieldWidth];
	//Изначально все поле свободно
	for (i = 0; i < FieldHeight; i++)
		for (j = 0; j < FieldWidth; j++)
			occupied[i][j] = 0;
	char stepCell[3];
	//Добавляем все корабли
	for (int s = 0; s < ship_count;)
	{
		//Если размещение автоматическое
		if (autoAlloc)
		{
			//Выбираем случайную позицию
			i = rand() % FieldHeight;
			j = rand() % FieldWidth;
			//Выбираем ориентацию расположения
			orientation = rand() % 2;
		}
		else
		{
			system("cls");
			cout << "Map of field (0 - free cell, 1 - occupied cell)" << endl;
			//Выводим строку цифр сверху
			cout << "  ";
			for (int i = 0; i < FieldWidth; i++)
				cout << i << " ";
			cout << endl;
			//Выводим столбец букв сбоку и таблицу где можно размещать корабли
			int ch = 'A';
			for (i = 0; i < FieldHeight; i++, ch++)
			{
				cout << (char)ch << " ";
				for (j = 0; j < FieldWidth; j++)
					cout << occupied[i][j] << " ";
				cout << endl;
			}
			//Выбераем ориеентацию для нашего корабля
			cout << "Adding " << ships[s] << "-deck ship." << endl;
			cout << "Enter ship orientation." << endl;
			char orient;
			for (;;)
			{
				cout << "h - horizontal, v - vertical: "; 
				cin >> orient;
				cin.clear(); // на случай, если предыдущий ввод завершился с ошибкой
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				if (orient == 'h')
				{
					orientation = 0;
					break;
				}
				else if (orient == 'v')
				{
					orientation = 1;
					break;
				}
			}
			//Выбираем начальную позицю для корабля
			for (;;)
			{
				cout << "Enter position to add ship: ";;
				//Считываем что ввел пользователь
				cin >> stepCell;
				//Иначе проверяем введенные данные
				i = (int)(stepCell[0] - 'A');
				j = (int)(stepCell[1] - '0');
				if ((i >= 0 && i <= 9) &&
					(j >= 0 && j <= 9) &&
					!occupied[i][j])
					break;
				else
					cout << "Can't addship at this position. Try Again." << endl;
			}
		}
		//Если она свободно двигаемся дальше
		if (!occupied[i][j])
		{
			//Если ориентация горизонтальная, то
			if (orientation == 0)
			{
				//Проверяем чтобы длина коробля не выходила за пределы поля
				if (j + ships[s] - 1 < FieldWidth)
					//Проверяем чтобы конец коробля находился на незанятой клетке
					if (occupied[i][j + ships[s] - 1] == 0)
					{
						//Добавляем блоки коробля
						for (int k = 0; k < ships[s]; k++)
						{
							//Добавление блока корабля
							field[i][j + k] = BLOCK;
							//Заполняем "карту занятости"
							occupied[i][j + k] = 1;
							//Добавление границ т.к. нельзя ставить один корабль вплотную к другому
							//Проверяем будут ли границы коробля выступать за пределы игрового поля
							//Если нет, то добавляем их
							//Это границы вдоль корабля
							if (i - 1 > 0)
								occupied[i - 1][j + k] = 1;
							if (i + 1 < FieldHeight)
								occupied[i + 1][j + k] = 1;
						}
						//Добавляем границы поперек корабля
						//Точно так же проверяем что мы работаем в пределах массива
						if (j - 1 > 0)
						{
							if (i - 1 > 0)
								occupied[i - 1][j - 1] = 1;
							if (i + 1 < FieldHeight)
								occupied[i + 1][j - 1] = 1;
							occupied[i][j - 1] = 1;
						}
						if (j + ships[s] < FieldWidth)
						{
							if (i - 1 > 0)
								occupied[i - 1][j + ships[s]] = 1;
							if (i + 1 < FieldHeight)
								occupied[i + 1][j + ships[s]] = 1;
							occupied[i][j + ships[s]] = 1;
						}
						//После этого можно переходить к следующему кораблю
						s++;
					}
			}
			else
			{
				//Точно такая же ветка как и выше
				//Только здесь работаем с вертикальным расположением
				//Поэтому расположениевсе действия происходят зеркально (теперь не с j а i)
				if (i + ships[s] - 1 < FieldHeight)
					if (occupied[i + ships[s] - 1][j] == 0)
					{
						for (int k = 0; k < ships[s]; k++)
						{
							if (j - 1 > 0)
								occupied[i + k][j - 1] = 1;
							if (j + 1 < FieldWidth)
								occupied[i + k][j + 1] = 1;
							occupied[i + k][j] = 1;
							field[i + k][j] = BLOCK;
						}
						if (i - 1 > 0)
						{
							if (j - 1 > 0)
								occupied[i - 1][j - 1] = 1;
							if (j + 1 < FieldWidth)
								occupied[i - 1][j + 1] = 1;
							occupied[i - 1][j] = 1;
						}
						if (i + ships[s] < FieldHeight)
						{
							if (j - 1 > 0)
								occupied[i + ships[s]][j - 1] = 1;
							if (j + 1 < FieldWidth)
								occupied[i + ships[s]][j + 1] = 1;
							occupied[i + ships[s]][j] = 1;
						}
						s++;
					}
			}
		}
	}
}

void DisplayGame(const GameState &game)
{
	//Очищаем экран
	system("cls");
	//Выводим информацию про игроков
	cout << "\t" << game.player1.name << " - " << game.player1.score << "\t\t\t" << game.ai.name << " - " << game.ai.score << endl;
	//Делаем "красивый" вывод
	//Выводим верхнюю строку цифр
	cout << "  ";
	for (int i = 0; i < FieldWidth; i++)
		cout << i << " ";
	cout << "\t\t  ";
	for (int i = 0; i < FieldWidth; i++)
		cout << i << " ";
	cout << endl;


	int ch = 'A';
	//Для каждой строки ниже цифр выводим поле
	for (int i = 0; i < FieldHeight; i++, ch++)
	{
		//Пометка о буквенном номере строки слева
		cout << (char)ch << " ";
		//Элеменеты на поле игрока-человека
		for (int j = 0; j < FieldWidth; j++)
			switch (game.playerField[i][j])
		{
			case EMPTY:		cout << "  "; break;
			case MISS:		cout << "* "; break;
			case BLOCK:		cout << "B "; break;
			case DESTROYED: cout << "x "; break;
		}
		//Пометка о буквенном номере строки слева
		cout << "\t\t" << (char)ch << " ";
		//Элеменеты на поле игрока-ИИ
		for (int j = 0; j < FieldWidth; j++)
			switch (game.aiField[i][j])
		{
			//В отличии от человека для блоков выводим пустоту, иначе будем видеть где корабли противника
			case EMPTY:
			case BLOCK:
				cout << "  ";
				break;
			case MISS:		cout << "* "; break;
			case DESTROYED: cout << "x "; break;
		}
		cout << endl;
	}
	//Выводи подсказки
	cout << "\"save\" for saving" << endl;
	cout << "\"stop\" for ending (results will not be saved)" << endl;
	cout << "Letter+number for action (i.g. A0)" << endl;
}

int PlayerMove(Cell field[][FieldWidth], int &AvailableShips, Player &player, bool ai)
{
	int i, j;
	//Переменная для хранения хода пользователя
	char stepCell[5];
	for (;;)
	{
		//Если игрок - ИИ, то берем случайные числа
		if (ai)
		{
			//Берем случайные координаты для хода ИИ
			i = rand() % FieldHeight;
			j = rand() % FieldWidth;
		}
		//Иначе игрок - человек
		else
		{
			//Берем координаты для хода пользователя
			//До тех пор пока он не введет их правильно
			for (;;)
			{
				//Считываем что ввел пользователь
				cin >> stepCell;
				//Если это слово save, то игра прекращается досрочно, но предлагает сохранится перед выходом
				if (strcmp(stepCell, "save") == 0)
					return -1;
				//Если это слово stop, то игра прекращается досрочно
				if (strcmp(stepCell, "stop") == 0)
					return -2;
				//Иначе проверяем введенные данные
				/*
				*	Клетка состоит из буквы и цифры, к примеру: A0
				*	буквы могут быть только от A до J, поэтому если вычести из них значение A и перевести в целое,
				*	то получим значение от 0 до 9
				*	Тоже самое касается цифр, т.е. из значения символьной перменной мы вычитаем символ (не цифру!) 0
				*	потом переводи полученный символ в целое и получаем целое число от 0 до 9
				*/
				i = (int)(stepCell[0] - 'A');
				j = (int)(stepCell[1] - '0');
				//Если данные введены правильно то продолжаем игру, иначе ждем правильно хода
				if ((i >= 0 && i <= 9) &&
					(j >= 0 && j <= 9))
					break;
				else
					cout << "Your turn is unvailable. Try Again." << endl;
			}
		}
		//Если ячейка поля была пустой, то один из игроков промазал
		if (field[i][j] == EMPTY)
		{
			//Ставим отметка промаха и вычитаем 5 очков из общего счета
			field[i][j] = MISS;
			player.score -= 5;
			return 0;
		}
		//Если ячейка была блоком коробля, то ставим ей отметку, что она уничтожена
		else if (field[i][j] == BLOCK)
		{
			field[i][j] = DESTROYED;
			//Если доступных кораблей в распоряжении противника больше 15, то прибавляем к счету +5
			//Если больше 10, то прибавляем к счету +10 и т.д.
			if (AvailableShips > 15)
				player.score += 5;
			else if (AvailableShips > 10)
				player.score += 10;
			else if (AvailableShips > 5)
				player.score += 15;
			else if (AvailableShips > 0)
				player.score += 20;
			//Делаем пометку что еще один блок уничтожен
			AvailableShips--;
			return 0;
		}
	}
}

int LoadGame(const char *filename, GameState &game)
{
	FILE *pFile;
	int tmp;
	//Открываем файл на чтение
	pFile = fopen(filename, "r");
	if (pFile == 0)
		return 0;
	//Считываем поле игрока
	for (int i = 0; i < FieldHeight; i++)
		for (int j = 0; j < FieldWidth; j++)
		{
			fscanf(pFile, "%d", &tmp);
			game.playerField[i][j] = (Cell)tmp;
		}
	//Считываем поле ИИ
	for (int i = 0; i < FieldHeight; i++)
		for (int j = 0; j < FieldWidth; j++)
		{
			fscanf(pFile, "%d", &tmp);
			game.aiField[i][j] = (Cell)tmp;
		}
	//Считываем данные про игрока
	fscanf(pFile, "%s %d %d", game.player1.name, &game.player1.score, &game.player1AvailableShips);
	//Считываем данные про ИИ
	fscanf(pFile, "%s %d %d", game.ai.name, &game.ai.score, &game.aiAvailableShips);
	fclose(pFile);
	return 1;
}

void SaveGame(const char *filename, const GameState &game)
{
	FILE *pFile;
	int tmp;
	//Открываем файл на запись
	pFile = fopen(filename, "w");
	//Записываем состояние поля игрока
	for (int i = 0; i < FieldHeight; i++)
	{
		for (int j = 0; j < FieldWidth; j++)
			fprintf(pFile, "%d ", ((int)game.playerField[i][j]));
		fprintf(pFile, "\n");
	}
	//Записываем состояние поля ИИ
	for (int i = 0; i < FieldHeight; i++)
	{
		for (int j = 0; j < FieldWidth; j++)
			fprintf(pFile, "%d ", ((int)game.aiField[i][j]));
		fprintf(pFile, "\n");
	}
	//Записываем информацию про игрока
	fprintf(pFile, "%s %d %d\n", game.player1.name, game.player1.score, game.player1AvailableShips);
	//Записываем информацию про ИИ
	fprintf(pFile, "%s %d %d\n", game.ai.name, game.ai.score, game.aiAvailableShips);
	fclose(pFile);
}

void SavePlayerScore(const Player &newPlayer)
{
	FILE *pFile;
	const char *filename = "score.txt";
	Player *players;
	int playersCount = 0;
	//Открываем файл для чтения и записи
	pFile = fopen(filename, "r+");
	//Если файл отсутствует
	if (!pFile)
	{
		//Создаем файл и открываем на запись
		pFile = fopen(filename, "w+");
		//Т.к. файл не был создан, то записываем что количество записей = 1
		fprintf(pFile, "1\n");
		//Записываем данные про нового игрока
		fprintf(pFile, "%s %d\n", newPlayer.name, newPlayer.score);
		//Закрываем файл
		fclose(pFile);
	}//Иначе
	else
	{
		//Считываем количество игроков
		fscanf(pFile, "%d", &playersCount);
		//Выделяем память для массива игроков на 1 больше
		players = new Player[playersCount + 1];
		//Считываем данные из файла
		for (int i = 0; i < playersCount; i++)
			fscanf(pFile, "%s %d\n", players[i].name, &players[i].score);
		//Добавляем нового игрока в конец
		players[playersCount] = newPlayer;
		playersCount++;
		/*
		* Сортировка пузырьком по убыванию счета
		*/
		for (int i = 0; i < playersCount - 1; i++)
			for (int j = 0; j < playersCount - 1 - i; j++)
				if (players[j].score < players[j + 1].score)
					swap(players[j], players[j + 1]);
		//Переходим в начало файла
		fseek(pFile, 0, SEEK_SET);
		//Записываем все данные обратно
		fprintf(pFile, "%d\n", playersCount);
		for (int i = 0; i < playersCount; i++)
			fprintf(pFile, "%s %d\n", players[i].name, players[i].score);
		//Закрываем файл
		fclose(pFile);
	}
}

void LoadPlayersScores(Player *&players, int &playersCount)
{
	FILE *pFile;
	//Предположим что файл с рекордами единый для всех
	const char *filename = "score.txt";
	playersCount = 0;
	players = nullptr;
	//Открываем файл
	pFile = fopen(filename, "r");
	//Если смогли открыть файл, то считываем данные иначе функция завершится с пустыми данными
	if (pFile)
	{
		//Считываем количество игроков в рекордах
		fscanf(pFile, "%d", &playersCount);
		//Выделяем память под массив игроков и их рекордов
		players = new Player[playersCount];
		//Читаем данные из файла
		for (int i = 0; i < playersCount; i++)
			fscanf(pFile, "%s %d", players[i].name, &players[i].score);
		fclose(pFile);
	}
	
}

void PrintPlayerScore(const Player *players, int playersCount)
{
	//Вывод данных про очки пользователей
	for (int i = 0; i < playersCount; i++)
		printf("%d. %s %d\n", (i + 1), players[i].name, players[i].score);
}
