#define FORCELINUX 1

#if defined(__unix__) || FORCELINUX
    #include <ncurses/curses.h>
#elif (defined(_WIN32) || defined(WIN32)) && !FORCELINUX
    #include <curses.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NAME_COUNT 10
#define MAX_NAME 5
#define ORDER_COUNT 10
#define MAX_ORDER 10

WINDOW *createWin(int h, int w, int y, int x) {
	WINDOW* win;
	win = newwin(h, w, y, x);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

WINDOW *createCenteredWindow(int h, int w) {
	int height = h;
  	int width = w;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    return createWin(height, width, starty, startx);
}

WINDOW *createCenteredWindowOff(int h, int w, int yoffset, int xoffset) {
	int height = h;
  	int width = w;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    return createWin(height, width, (starty + yoffset), (startx + xoffset));
}

void destroyWin(WINDOW * win) {
	wclear(win);
	wrefresh(win);
	delwin(win);
    free(win);
}

int vOffset(int offset) {
    return (LINES / 2) + offset;
}

int hOffset(int offset) {
    return (COLS / 2) + offset;
}

///////////////////////////
// Stack
///////////////////////////

typedef struct {
	char value[MAX_ORDER];
} Order;


typedef struct Node {
	Order* value;
	struct Node* next;
} Node;

typedef struct {
	Node* top;
} Stack;

Stack* dsinit() {
    Stack* stk = (Stack *) malloc(sizeof(Stack));
    stk->top=NULL;
	return stk;
}

void dspush(Stack* ds, Order* value) {
	Node* node = (Node *) malloc(sizeof(Node));
	node->value = value;
	node->next = ds->top;
	ds->top = node;
}

Order* dspop(Stack *ds) {
	Node * top = ds->top;
	if(top == NULL) {
		return NULL;
	}
	ds->top = top->next;
	Order* value = top-> value;
	free(top);
	return value;
}

Order* dspeek(Stack *ds) {
    if (ds->top == NULL) {
        return NULL;
    }
	return ds->top->value;
}

///////////////////////////
// QUEUE
///////////////////////////

typedef struct {
	char name[MAX_NAME];
	Stack* orders;
} Client;

typedef struct QN {
    Client* client;
    struct QN *prev;
} QNode;

typedef struct {
    QNode *front;
    QNode *back;
    int count;
} Queue;

Queue initQueue() {
    return (Queue) {.front=NULL, .back=NULL, .count=0};
}

void qEnqueue(Queue* q, Client* client) {
    q->count++;
    QNode* qn = (QNode *) malloc(sizeof(QNode));
    qn->client = client;
    qn->prev = NULL;
    if(q->front == NULL) {
        q->front = qn;
        q->back = qn;
        return;
    }
    q->back->prev=qn;
    q->back=qn;
}

Client* qDequeue(Queue* q) {
    q->count--;
    QNode* front = q->front;
    if(front == NULL) {
        return NULL;
    }
    Client* value = front->client;
    q->front = front->prev;
    free(front);
    return value;
}

Client* qPeek(Queue* q) {
    if(q->front == NULL) {
        return 0;
    }
    return q->front->client;
}

//////////////////////////////////

void qAddClient(Queue* q, char *name, Stack *orders) {
	Client* client = (Client *) malloc(sizeof(Client));
	strncpy(client->name, name, MAX_NAME);
	client->orders = orders;
	qEnqueue(q, client);
}

const char orderBank[ORDER_COUNT][MAX_ORDER] = {
	"bum", "lettuce", "burger", "cheese", "cyanide",
	"mayo", "tomato", "air", "hammm", "fries"
};

Stack* createOrders() {
	Stack* st = dsinit();
	int orderNum = 1 + (rand() % 5);
	for(int i = 0; i < orderNum; i++) {
		int orderId = rand() % ORDER_COUNT;
		Order* order = (Order *) malloc(sizeof(Order));
		strncpy(order->value, orderBank[orderId], MAX_ORDER);
		dspush(st, order);
	}
	return st;
}

const char nameBank[NAME_COUNT][MAX_NAME] = {
    "bob", "dale", "trum", "laur", "earl",
    "peak", "ben", "audr", "llad", "twin"
};

Client* createClient() {
    Client* client = (Client *) malloc(sizeof(Client));
    int nameId = rand() % NAME_COUNT;
    strncpy(client->name, nameBank[nameId], MAX_NAME);
    client->orders = createOrders();
    return client;
}

///////////////////////////

void showTempMessage(int h, int w, char *message) {
	WINDOW *tempwin = createCenteredWindow(h , w);
	box(tempwin, 0, 0);
	mvwprintw(tempwin, 1, 1, "%s", message);
	wrefresh(tempwin);
	wgetch(tempwin);
	destroyWin(tempwin);
}

void showTempMessageS(char* message) {
    showTempMessage(3, strlen(message), message);
}

void updateQWin(WINDOW* qwin, Queue* q) {
    wclear(qwin);
    mvwprintw(qwin, 1, 1, " START <---");
    int qCount = 0;
    QNode* prev = q->front;
    while(prev != NULL) {
        prev = prev->prev;
        qCount++;
    } 
    box(qwin, 0, 0);
    wrefresh(qwin);
}

void showClientWin(WINDOW* cliwin, Queue* clients) {
	wclear(cliwin);
	int cliCount = 0;
	QNode* n = clients->front;
	while(n != NULL) {
		mvwprintw(cliwin, 1 + (cliCount), 1, "%s", n->client->name);
		cliCount++;
		n = n->prev;
	}
	box(cliwin, 0, 0);
	wrefresh(cliwin);
}

void showOrderWin(WINDOW* orderWin, Stack* orders) {
    wclear(orderWin);
    Node* next = orders->top;
    int i = 0;
    wmove(orderWin, 1, 1);

    while(next != NULL) {
        wprintw(orderWin, "%s | ", next->value->value);
        next = next->next;
        i++;
        if(i >= 5) {
            break;
        }
        refresh();
    }
    // mvwprintw(orderWin, 1, 1, "Something");
    box(orderWin, 0, 0);
    mvwprintw(orderWin, 0, 1, "Current Order: ");
    wrefresh(orderWin);
}

void updateCurOrderWin(WINDOW *orderwin, Order* order) {
    wclear(orderwin);
    mvwprintw(orderwin, 1, 1, " %s ", order->value);
    box(orderwin, 0, 0);
    mvwprintw(orderwin, 0, 1, "Ing:");
    wrefresh(orderwin);
}

void updateInputWin(WINDOW* inputwin, char* input, bool error) {
    if(error) {
        wattron(inputwin, COLOR_PAIR(2));
    } else {
        wattron(inputwin, COLOR_PAIR(1));
    }
    wclear(inputwin);
    mvwprintw(inputwin, 1, 1, " %s ", input);
    box(inputwin, 0, 0);
    mvwprintw(inputwin, 0, 1, "Input:");
    wrefresh(inputwin);
}

int calcTimeDiff(int clientCount) {
    int timediff = 1 + (rand() % 4);
    if (clientCount > 3) {
        return timediff + ((clientCount / 2) - clientCount / 3.0);
    }
    return timediff;
}

void gameLoop(Queue *clients) {
    bool finishedClient = true;
    bool finishedOrder = true;

    Client* cli = createClient();
    qEnqueue(clients, cli);

    Client* curClient = NULL;
    Stack* curOrders = NULL;

	WINDOW* cliwin = createWin(LINES - 4, MAX_NAME + 4, 2, COLS - (MAX_NAME + 10));
	WINDOW* ordersWin = createWin(3, COLS - 40, 13, 20);
    WINDOW* curorderwin = createCenteredWindow(3, MAX_ORDER + 4);
    WINDOW* inputwin = createCenteredWindowOff(3, MAX_ORDER + 4, + 4, 0);

    wtimeout(inputwin, 1000);

    char input[MAX_ORDER];
    int inputCount = 0;
    bool hasInputError = false;
    Order* curOrder = NULL;

    time_t baset = time(NULL);
    int timediff = 0;

    for(;;) {
        time_t now = time(NULL);
        if ((now - baset) > timediff) {
            Client* cli = createClient();
            qEnqueue(clients, cli);
            showClientWin(cliwin, clients);
            baset = now;
            timediff = calcTimeDiff(clients->count);
        }
        if(finishedClient) {
            if(curClient != NULL) {
                free(curClient);
            }
            curClient = qDequeue(clients);
            if(curClient == NULL) {
                break;
            }
            showClientWin(cliwin, clients);
            curOrders = curClient->orders;
            finishedClient = false;
        }
        if(finishedOrder) {
            if(curOrder != NULL) {
                free(curOrder);
            }
            curOrder = dspop(curOrders);
            if(curOrder == NULL) {
                finishedClient = true;
                continue;
            }
            showOrderWin(ordersWin, curOrders);
            updateCurOrderWin(curorderwin, curOrder);
            finishedOrder = false;
        }
        updateInputWin(inputwin, input, hasInputError);
        char chinput = wgetch(inputwin);

        // For Debug
        //printw("%d %c", chinput, chinput);
        //refresh();

        switch(chinput) {
            case 7:
            case '\b':
                if(inputCount <= 0) {
                    continue;
                }
                hasInputError = false;
                input[--inputCount] = '\0';
                break;
            case '\n':
                int diff = strcmp(input, curOrder->value);
                if(diff == 0) {
                    finishedOrder = true;
                    strncpy(input, "", MAX_ORDER);
                    inputCount = 0;
                } else {
                    hasInputError = true;
                }
                break;
            case 0:
            case -1:
                break;
            default:
                if(inputCount >= MAX_ORDER) {
                    continue;
                }
                hasInputError = false;
                input[inputCount++] = chinput;
                input[inputCount] = '\0';
        }
    }
    destroyWin(inputwin);
    destroyWin(curorderwin);
    destroyWin(ordersWin);
    destroyWin(cliwin);

    char *msg = malloc(sizeof(char) * 50);
    strncpy(msg, "You Won?", 50);
    showTempMessage(3, strlen(msg) + 2, msg);
    free(msg);
}

int main(){
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(false);
    srand(time(NULL));
    start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	Queue clients = initQueue();
    gameLoop(&clients);

	refresh();
	getch();
	endwin();
	return 0;
}