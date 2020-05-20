#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RANK: rank(); break;
		case '3' : recommendedPlay(); break;
		default: break;
		}
	}
	
	writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	nextBlock[3]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	recRoot = (rptr)malloc(sizeof(RecNode));
	recRoot->lv =0;
	recRoot->score = 0;
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			recRoot->f[j][i] = 0;
	recommend(recRoot);
	

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	move(9,WIDTH+10);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i,WIDTH+13);
		for( j = 0 ; j < 4 ; j++){
			if( block[nextBlock[2]][0][i][j] == 1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
	
		
		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	for(i=0 ; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(blockX + j <= -1 || blockX + j >= 10)
					return 0;
				else if(blockY + i >= 22)
					return 0;
				else if(f[i+blockY][j+blockX])
					return 0;
			}
		}
	}
	return 1;
}
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);


	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig){
	// user code
	int i,j;
	if(CheckToMove(field,nextBlock[0],blockRotate, blockY+1, blockX)){
		blockY++;
		DrawField();
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	}
	else{
		if(blockY == -1)
			gameOver = 1;
		score += AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score += DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = nextBlock[3];
		nextBlock[3] = rand()%7;

		

		recRoot->lv = 0;
		recRoot->score = 0;
		/*
		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				recRoot->f[i][j] = field[i][j];
		*/
		memcpy(recRoot->f,field,sizeof(char[HEIGHT][WIDTH])); // memcpy 로 속도 향상
		modified_recommend(recRoot);
		//recommend(recRoot);
		if(recommendY == -1)
			gameOver = 1;
		
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH/2 -2;

		DrawNextBlock(nextBlock);
		DrawField();
		PrintScore(score);	
	}


	timed_out = 0;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j,touch=0;
	
	for(i=0; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				f[i+blockY][j+blockX] = 1;
				if(blockY + i == 21)
					touch++;
			}
		}
	}
	return touch*10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i,line=0,j,k;
	int count;
	
	for(i=HEIGHT-1; i >= 0 ; i--){
		count =0;
		for(j=0; j < WIDTH ; j++){
			if(f[i][j] == 1)
				count++;
			else
				continue;
		}
		if(count == 0)
			break;
		if(count == WIDTH){
			line++;
			for(j=i; j > 0 ; j--){
				memcpy(f[j],f[j-1],sizeof(char[WIDTH])); // for -> memcpy
				
			}
			i++;
		}
	}
	
	return line*line*100;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int tarY = y;
	while(CheckToMove(field,blockID,blockRotate,++tarY,x)!=0);
	DrawBlock(--tarY,x,blockID,blockRotate,'/');

}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,' ');
	DrawShadow(y,x,blockID,blockRotate);
	DrawRecommend(recommendY,recommendX,blockID,recommendR);

}

void createRankList(){
	FILE * fp;
	int i,j,n;
	nptr new;

	fp = fopen("rank.txt","r");

	//init rankData 
	rankData = (Usr_rank *)malloc(sizeof(Usr_rank));
	rankData->cnt = 0;
	rankData->front = NULL;
	rankData->rear = NULL;

	if(fscanf(fp,"%d",&n) != EOF){
		for(i=1; i<=n ; i++){
			new = (NODE *)malloc(sizeof(NODE));
			fscanf(fp,"%s %d",new->name,&new->score);
			new->next = NULL;
			if(i==1)
				rankData->front = new;
			else
				rankData->rear->next = new;
			rankData->rear = new;
			(rankData->cnt)++;
		}
	}
	else{
		fclose(fp);
		fp = fopen("rank.txt","w");
		fprintf(fp,"%d",rankData->cnt);


	}
	
	fclose(fp);
	
}

void rank(){
	int X = 1, Y = rankData->cnt;
	int ch,i,j,flag;
	char str[NAMELEN];
	clear();
	
	move(0,0);
	printw("1. list ranks from X to Y");
	move(1,0);
	printw("2. list ranks by a specific name");
	move(2,0);
	printw("3. delete a specific rank");

	ch = wgetch(stdscr);
	if( ch == '1'){
		move(3,0);
		echo();
		printw("X: ");
		scanw("%d",&X);
		printw("Y: ");
		scanw("%d",&Y);
		noecho();
		printw("\tname\t\t| score\t\n");
		printw("-----------------------------------\n");
		if( X > Y || rankData->cnt == 0 || X > rankData->cnt)
			mvprintw(8,0, "search failure: no rank in the list\n");
		else{
			nptr now;
			now = rankData->front;

			for(i = 1 ; i < X ; i++)
				now = now->next;
			for(i = 0 ; i <= Y - X ; i++){
				printw("%-16s\t| %d\n",now->name,now->score);
				now= now->next;
			}
		

		}

	}
	else if( ch == '2'){
		move(3,0);
		echo();
		printw("Input the name : ");
		scanw("%s",str);
		noecho();

		printw("\tname\t\t| score\t\n");
		printw("-----------------------------------\n");

		flag = 0;
		nptr now = rankData->front;

		for(i=1 ; i <= rankData->cnt ; i++){
			if(strcmp(now->name,str) == 0){
				printw("%-16s\t| %d\n",now->name,now->score);
				flag = 1;
			}
			now = now->next;
		}
		if(flag == 0)
			mvprintw(8,0, "search failure: no name in the list\n");
	}
	else if( ch == '3'){
		move(3,0);
		echo();
		printw("Input the rank: ");
		scanw("%d",&j);
		noecho();

		if(j >=1 && j <= rankData->cnt){
			nptr now = rankData->front;
			nptr del;
			if( j == 1){
				rankData->front = now->next;
				free(now);
			}
			else{
				for(i=1 ; i < j-1 ; i++)
					now = now->next;
				del = now->next;
				now->next = del->next;
				free(del);
				if( j == rankData->cnt)
					rankData->rear = now;
			}
			rankData->cnt--;
			mvprintw(5,0, "result: the rank deleted\n");
		}
		else
			mvprintw(5,0, "search failure: the rank not in the list\n");





	}

	getch();


	// user code
}

void writeRankFile(){
	FILE * fp;
	int i;
	nptr now = rankData->front;
	fp = fopen("rank.txt","w");

	fprintf(fp,"%d\n",rankData->cnt);

	for(i=1;i<=rankData->cnt;i++){
		fprintf(fp,"%s %d\n",now->name,now->score);
		now = now->next;
	}
	fclose(fp);

	return;

}

void newRank(int score){
	char str[NAMELEN+1];
	int i,j;
	nptr new,pre, now;

	clear();
	move(0,0);
	printw("your name : ");
	scanw("%s", str);

	new = (nptr)malloc(sizeof(NODE));
	now = rankData->front;
	pre = now;

	new->score = score;
	new->next = NULL;
	strcpy(new->name,str);
	
	if( rankData->cnt == 0){
		rankData->front = new;
		rankData->rear = new;
	}
	else{
		for(i=1 ; i <= rankData->cnt ; i++){
			if(score >= now->score){
				if( i == 1){
					new->next = now;
					rankData->front = new; 
				}
				else{
					pre->next = new;
					new->next = now;
				}
				break;
				
			}
			pre = now;
			if( i != rankData->cnt)
				now = now->next;
		}
		if(i == rankData->cnt + 1){
			now->next = new;
			rankData->rear = new;

		}
	}
	rankData->cnt++;

	return;

}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,'R');
	return ;
}


int checkScore(char f[HEIGHT][WIDTH], int y, int x, int blockID, int blockRotate){
	int Score=0;


	Score += DeleteLine(f);




}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int startx,endx,x,y,r;
	int numCh=0,i,j;
	
	recommendX = 0;
	recommendY = 0;
	recommendR = 0;

		
	rptr child;
	rptr curr;
	for(r = 0,numCh = 0 ; r < NUM_OF_ROTATE ; r++){
		startx = 1;
		endx = 6;
		while(CheckToMove(root->f,nextBlock[root->lv],r,0,--startx));
		startx++;
		while(CheckToMove(root->f,nextBlock[root->lv],r,0,++endx));
		endx--;
		for(x = startx ; x <= endx ; x++){
			y=2;
			while(CheckToMove(root->f,nextBlock[root->lv],r,++y,x));
			y--;
			child = (rptr)malloc(sizeof(RecNode));
			child->lv = root->lv + 1;
			child->score = root->score;
			child->rx = x;
			child->ry = y;
			child->rr = r;
			
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					child->f[i][j] = root->f[i][j];
			child->score += RAddBlockToField(child->f,nextBlock[root->lv],r,y,x);
			child->score += DeleteLine(child->f);

			root->c[numCh] = child;

			if(child->lv < VISIBLE_BLOCKS)
				child->score = recommend(child);
		
			if(child->score >= max){
				max = child->score;
			}
			
			numCh++;
		}
	}
	if(root->lv ==0){
		for(i=0;i < numCh; i++){
			if(root->c[i]->score == max){
				recommendX = root->c[i]->rx;
				recommendY = root->c[i]->ry;
				recommendR = root->c[i]->rr;

			}
		}
		
	}

	return max;
}
int modified_recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int startx,endx,x,y,r;
	int numCh=0,i,j,rot,block;
	
	recommendX = 0;
	recommendY = -1;
	recommendR = 0;

		
	rptr child;
	rptr curr;

	block = nextBlock[root->lv];
	if(block == 4)
		rot = 1;
	else if(block == 0 || block == 5 || block == 6)
		rot = 2;
	else
		rot = 4;
	for(r = 0,numCh = 0 ; r < rot ; r++){
		startx = 1;
		endx = 6;
		while(CheckToMove(root->f,nextBlock[root->lv],r,0,--startx));
		startx++;
		while(CheckToMove(root->f,nextBlock[root->lv],r,0,++endx));
		endx--;
		for(x = startx ; x <= endx ; x++, numCh++){

			child = (rptr)malloc(sizeof(RecNode));
			root->c[numCh] = child;
			
			y=-1;
			if(CheckToMove(root->f,nextBlock[root->lv],r,y,x) == 0)
				continue;
			while(CheckToMove(root->f,nextBlock[root->lv],r,y,x)) y++;
			y--;
			
			child->lv = root->lv + 1;
			child->score = root->score;
			child->rx = x;
			child->ry = y;
			child->rr = r;
			/*
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					child->f[i][j] = root->f[i][j];
			*/
			memcpy(child->f,root->f,sizeof(char[HEIGHT][WIDTH]));
			child->score += RAddBlockToField(child->f,nextBlock[root->lv],r,y,x);
			child->score += DeleteLine(child->f);
			child->score += y*10 - child->lv;



			if(child->lv < VISIBLE_BLOCKS)
				child->score = modified_recommend(child);
		
			if(child->score >= max){
				max = child->score;
			}
			
		}
	}
	
	if(root->lv ==0){
		for(i=0;i < numCh; i++){
			if(root->c[i]->score == max){
				recommendX = root->c[i]->rx;
				recommendY = root->c[i]->ry;
				recommendR = root->c[i]->rr;

			}
		}
		for(i=0;i < numCh; i++){
			curr = root->c[i];
			free(curr);
		}
	}
	else{
		for(i=0; i < numCh ; i++){
			curr = root->c[i];
			free(curr);

		}

	}
	
	// user code

	return max;
}

int RAddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j,touch=0;
	for(i=0; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(f[i+blockY+1][j+blockX])
					touch++;
			}
		}
	}
	for(i=0; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				f[i+blockY][j+blockX] = 1;
				if(blockY + i == 21)
					touch++;
			}
		}
	}
	return touch*10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

void recommendedPlay(){
	int command,start;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	start = time(NULL);
	do{
		if(timed_out==0){

			ualarm(1000,100);
		
			timed_out = 1;			
			
			//DrawField();
			/*
			if(CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)==0){
				gameOver = 1;
				timed_out = 0;
			}
			*/
			blockY = recommendY;
			blockX = recommendX;
			blockRotate = recommendR;

			
			mvprintw(25,1,"time : %d",time(NULL) - start);

			
		}


		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;

		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
	// user code
}
