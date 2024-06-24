//最終課題
//2023/07/31

#include <curses.h>
#include <cstdlib>
#include <time.h>
//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

//盤面状態
class PuyoArray{
private:
  puyocolor *data;
  unsigned int data_line;
  unsigned int data_column;

  //メモリ開放
  void Release(){
	if (data == NULL) {
		return;
	}
	delete[] data;
	data = NULL;
  }

public:  
    //盤面サイズ変更
    void ChangeSize(unsigned int line, unsigned int column)
    {
        Release();

        //新しいサイズでメモリ確保
        data = new puyocolor[line*column];
        data_line = line;
        data_column = column;
    }

    //盤面の行数を返す
    unsigned int GetLine(){return data_line;}

    //盤面の列数を返す
    unsigned int GetColumn(){return data_column;}

    //盤面の指定された位置の値を返す
    puyocolor GetValue(unsigned int y, unsigned int x){
        if (y >= GetLine() || x >= GetColumn())
        {
            //引数の値が正しくない
            return NONE;
        }
        return data[y*GetColumn() + x];
    }

    //盤面の指定された位置に値を書き込む
    void SetValue(unsigned int y, unsigned int x, puyocolor value){
        if (y >= GetLine() || x >= GetColumn())
        {
            //引数の値が正しくない
            return;
        }
        data[y*GetColumn() + x] = value;
    }

    //盤面のぷよの数
    int CountPuyo(){
        int cnt = 0;
        for (int y = 0; y < GetLine(); y++){
            for (int x=0; x < GetColumn(); x++){
                if (GetValue(y,x) != NONE){
                    cnt++;
                }
            }
        }
        return cnt;
    }

    PuyoArray():data(NULL), data_line(0), data_column(0){}
    ~PuyoArray(){Release();} 
};

class PuyoArrayActive: public PuyoArray{
private:
	int puyorotate;
public:
	int Rotate_getter(){
		return puyorotate;
	}

	void Rotate_setter(int p){
		puyorotate = p;
	}
	PuyoArrayActive():puyorotate(0){}
};
class PuyoArrayStack: public PuyoArray{};

class PuyoControl{

private:
	puyocolor newpuyo1, newpuyo2, nextpuyo1, nextpuyo2, next_nextpuyo1, next_nextpuyo2;
public:
	puyocolor next_puyo_array[4];
  	
	//コンストラクタでぷよを４つ生成する
	PuyoControl()
	{	
		srand((unsigned int)time(NULL));
		puyocolor color_array[4] = {RED, BLUE, GREEN, YELLOW};			
		nextpuyo1 = color_array[rand() % 4];
		nextpuyo2 = color_array[rand() % 4];
		next_nextpuyo1 = color_array[rand() % 4];
		next_nextpuyo2 = color_array[rand() % 4];

	}
	//盤面に新しいぷよ生成
  	void GeneratePuyo(PuyoArrayActive &active)
		{
			puyocolor color_array[4] = {RED, BLUE, GREEN, YELLOW};

			newpuyo1 = nextpuyo1;
			newpuyo2 = nextpuyo2;
			nextpuyo1 = next_nextpuyo1;
			nextpuyo2 = next_nextpuyo2;

			next_nextpuyo1 = color_array[rand() % 4];
			next_nextpuyo2 = color_array[rand() % 4];

			next_puyo_array[0] = nextpuyo1;
			next_puyo_array[1] = nextpuyo2;
			next_puyo_array[2] = next_nextpuyo1;
			next_puyo_array[3] = next_nextpuyo2;
	
			active.SetValue(0, 5, newpuyo1);
			active.SetValue(0, 6, newpuyo2);

			active.Rotate_setter(0);
		}

  //ぷよの着地判定．着地判定があるとtrueを返す
  	bool LandingPuyo(PuyoArrayActive &activepuyo, PuyoArrayStack &stackpuyo){
		bool landed = false;

		for (int y = activepuyo.GetLine() - 1; 0 <= y; y--){
			for (int x = activepuyo.GetColumn() -1; 0 <= x; x--){	
				//最下段着地判定
				if (activepuyo.GetValue(y, x) != NONE && y == activepuyo.GetLine() - 1 ){
					landed = true;
					//着地判定されたぷよを消す．本処理は必要に応じて変更する．
					stackpuyo.SetValue(y,x,activepuyo.GetValue(y,x));
					activepuyo.SetValue(y, x, NONE);
				}
				//着地積みぷよの上に着地判定
				if(y < activepuyo.GetLine() - 1 && activepuyo.GetValue(y,x)!= NONE && stackpuyo.GetValue(y+1,x) != NONE){
					landed = true;
					stackpuyo.SetValue(y,x,activepuyo.GetValue(y,x));
					activepuyo.SetValue(y, x, NONE);
				}
			}
		}
		return landed;
  	}

  	//左移動
  	void MoveLeft(PuyoArrayActive &activepuyo, PuyoArrayStack &stackpuyo){
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[activepuyo.GetLine()*activepuyo.GetColumn()];

		for (int i = 0; i < activepuyo.GetLine()*activepuyo.GetColumn(); i++){
			puyo_temp[i] = NONE;
		}

		//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = activepuyo.GetLine()-1; y >= 0; y--){
			for (int x = 0; x < activepuyo.GetColumn(); x++){	
				//１つ左にぷよが設置されていれば動けない
				if(0<x && activepuyo.GetValue(y,x) != NONE && stackpuyo.GetValue(y,x-1) != NONE){
					return;
				}			
				//１つ左にぷよが設置されていなければコピー
				else{
					if (activepuyo.GetValue(y,x) == NONE) {
						continue;
					}
					if (0 < x && activepuyo.GetValue(y, x-1) == NONE){
						puyo_temp[y*activepuyo.GetColumn() + (x - 1)] = activepuyo.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						activepuyo.SetValue(y, x, NONE);
					}
					else{
						puyo_temp[y*activepuyo.GetColumn() + x] = activepuyo.GetValue(y, x);
					}
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー
		for (int y = 0; y < activepuyo.GetLine(); y++){
			for (int x = 0; x < activepuyo.GetColumn(); x++){	
				activepuyo.SetValue(y, x, puyo_temp[y*activepuyo.GetColumn() + x]);
			}
		}

		//一時的格納場所メモリ解放
		delete[] puyo_temp;
  	}

  //右移動
  	void MoveRight(PuyoArrayActive &activepuyo, PuyoArrayStack &stackpuyo)
  	{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[activepuyo.GetLine()*activepuyo.GetColumn()];

		for (int i = 0; i < activepuyo.GetLine()*activepuyo.GetColumn(); i++){
			puyo_temp[i] = NONE;
		}

		//1つ右の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = activepuyo.GetLine()-1; y >= 0; y--){
			for (int x = activepuyo.GetColumn() - 1; x >= 0; x--){
				//１つ右にぷよが設置されていれば動けない
				if(0<x && activepuyo.GetValue(y,x) != NONE && stackpuyo.GetValue(y,x+1) != NONE){
					return;
				}
				//１つ右にぷよが設置されていなければコピー
				else{
					if (activepuyo.GetValue(y, x) == NONE){
						continue;
					}
					if (x < activepuyo.GetColumn() - 1 && activepuyo.GetValue(y, x+1) == NONE){
						puyo_temp[y*activepuyo.GetColumn() + (x+1)] = activepuyo.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						activepuyo.SetValue(y, x, NONE);
					}
					else{
						puyo_temp[y*activepuyo.GetColumn() + x] =activepuyo.GetValue(y, x);
					}
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー
		for (int y = 0; y <activepuyo.GetLine(); y++){
			for (int x = 0; x <activepuyo.GetColumn(); x++){
				activepuyo.SetValue(y, x, puyo_temp[y*activepuyo.GetColumn() + x]);
			}
		}

		//一時的格納場所メモリ解放
		delete[] puyo_temp;
  	}

  	//下移動
  	void MoveDown(PuyoArrayActive &activepuyo, PuyoArrayStack &stackpuyo)
  	{
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[activepuyo.GetLine()*activepuyo.GetColumn()];

    for (int i = 0; i < activepuyo.GetLine()*activepuyo.GetColumn(); i++)
      {
		puyo_temp[i] = NONE;
      }

    //1つ下の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = activepuyo.GetLine() - 1; y >= 0; y--){
		for (int x = 0; x < activepuyo.GetColumn(); x++)
		{
			if (activepuyo.GetValue(y, x) == NONE) {
				continue;
			}
			if (y < activepuyo.GetLine() - 1 && activepuyo.GetValue(y + 1, x) == NONE 
				&& stackpuyo.GetValue(y + 1, x) == NONE 
				&& (activepuyo.GetValue(y, x + 1) == NONE || stackpuyo.GetValue(y + 1, x + 1) == NONE) 
				&& (activepuyo.GetValue(y, x - 1) == NONE || stackpuyo.GetValue(y + 1, x - 1) == NONE)){
				puyo_temp[(y + 1)*activepuyo.GetColumn() + x] = activepuyo.GetValue(y, x);
				//コピー後に元位置のpuyoactiveのデータは消す
				activepuyo.SetValue(y, x, NONE);
			}
			else{
				puyo_temp[y*activepuyo.GetColumn() + x] = activepuyo.GetValue(y, x);
			}
			
		}
    }

    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < activepuyo.GetLine(); y++)
      {
		for (int x = 0; x < activepuyo.GetColumn(); x++)
		{
			activepuyo.SetValue(y, x, puyo_temp[y*activepuyo.GetColumn() + x]);
		}
      }

    //一時的格納場所メモリ解放
    delete[] puyo_temp;
  	}

	//ぷよ消滅処理を全座標で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack)
	{
		int vanishednumber = 0;
		for (int y = 0; y < puyostack.GetLine(); y++)
		{
			for (int x = 0; x < puyostack.GetColumn(); x++)
			{
				vanishednumber += VanishPuyo(puyostack, y, x);
			}
		}

		return vanishednumber;
	}

	//ぷよ消滅処理を座標(x,y)で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack, unsigned int y, unsigned int x)
	{
		//判定個所にぷよがなければ処理終了
		if (puyostack.GetValue(y, x) == NONE)
		{
			return 0;
		}


		//判定状態を表す列挙型
		//NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
		enum checkstate{ NOCHECK, CHECKING, CHECKED };

		//判定結果格納用の配列
		enum checkstate *field_array_check;
		field_array_check = new enum checkstate[puyostack.GetLine()*puyostack.GetColumn()];

		//配列初期化
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			field_array_check[i] = NOCHECK;
		}

		//座標(x,y)を判定対象にする
		field_array_check[y*puyostack.GetColumn() + x] = CHECKING;

		//判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
		bool checkagain = true;
		while (checkagain)
		{
			checkagain = false;

			for (int yy = 0; yy < puyostack.GetLine(); yy++)
			{
				for (int xx = 0; xx < puyostack.GetColumn(); xx++)
				{
					//(xx,yy)に判定対象がある場合
					if (field_array_check[yy*puyostack.GetColumn() + xx] == CHECKING)
					{
						//(xx+1,yy)の判定
						if (xx < puyostack.GetColumn() - 1)
						{
							//(xx+1,yy)と(xx,yy)のぷよの色が同じで，(xx+1,yy)のぷよが判定未実施か確認
							if (puyostack.GetValue(yy, xx + 1) == puyostack.GetValue(yy, xx) && field_array_check[yy*puyostack.GetColumn() + (xx + 1)] == NOCHECK)
							{
								//(xx+1,yy)を判定対象にする
								field_array_check[yy*puyostack.GetColumn() + (xx + 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(xx-1,yy)の判定
						if (xx > 0)
						{
							if (puyostack.GetValue(yy, xx - 1) == puyostack.GetValue(yy, xx) && field_array_check[yy*puyostack.GetColumn() + (xx - 1)] == NOCHECK)
							{
								field_array_check[yy*puyostack.GetColumn() + (xx - 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy+1)の判定
						if (yy < puyostack.GetLine() - 1)
						{
							if (puyostack.GetValue(yy + 1, xx) == puyostack.GetValue(yy, xx) && field_array_check[(yy + 1)*puyostack.GetColumn() + xx] == NOCHECK)
							{
								field_array_check[(yy + 1)*puyostack.GetColumn() + xx] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy-1)の判定
						if (yy > 0)
						{
							if (puyostack.GetValue(yy - 1, xx) == puyostack.GetValue(yy, xx) && field_array_check[(yy - 1)*puyostack.GetColumn() + xx] == NOCHECK)
							{
								field_array_check[(yy - 1)*puyostack.GetColumn() + xx] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy)を判定済みにする
						field_array_check[yy*puyostack.GetColumn() + xx] = CHECKED;
					}
				}
			}
		}

		//判定済みの数をカウント
		int puyocount = 0;
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			if (field_array_check[i] == CHECKED)
			{
				puyocount++;
			}
		}

		//4個以上あれば，判定済み座標のぷよを消す
		int vanishednumber = 0;
		if (4 <= puyocount)
		{
			for (int yy = 0; yy < puyostack.GetLine(); yy++)
			{
				for (int xx = 0; xx < puyostack.GetColumn(); xx++)
				{
					if (field_array_check[yy*puyostack.GetColumn() + xx] == CHECKED)
					{
						puyostack.SetValue(yy, xx, NONE);
						vanishednumber++;
					}
				}
			}
		}

		//メモリ解放
		delete[] field_array_check;
		return vanishednumber;
	}

	void Rotate(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack)
	{
		//フィールドをラスタ順に探索（最も上の行を左から右方向へチェックして，次に一つ下の行を左から右方向へチェックして，次にその下の行・・と繰り返す）し，先に発見される方をpuyo1, 次に発見される方をpuyo2に格納
		puyocolor puyo1, puyo2;
		int puyo1_x = 0;
		int puyo1_y = 0;
		int puyo2_x = 0;
		int puyo2_y = 0;
		

		bool findingpuyo1 = true;
		for (int y = 0; y < puyoactive.GetLine(); y++)
		{
			for (int x = 0; x < puyoactive.GetColumn(); x++)
			{
				if (puyoactive.GetValue(y, x) != NONE)
				{
					if (findingpuyo1)
					{
						puyo1 = puyoactive.GetValue(y, x);
						puyo1_x = x;
						puyo1_y = y;
						findingpuyo1 = false;
					}
					else
					{
						puyo2 = puyoactive.GetValue(y, x);
						puyo2_x = x;
						puyo2_y = y;
					}
				}
			}
		}


		//回転前のぷよを消す
		puyoactive.SetValue(puyo1_y, puyo1_x, NONE);
		puyoactive.SetValue(puyo2_y, puyo2_x, NONE);


		//操作中ぷよの回転
		switch (puyoactive.Rotate_getter())
		{
		case 0:
			//回転パターン
			//RB -> R
			//      B
			//Rがpuyo1, Bがpuyo2
			if (puyo2_x <= 0 || puyo2_y >= puyoactive.GetLine() - 1)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}
			if (puyostack.GetValue(puyo1_y+1,puyo1_x) != NONE){     //もし回転した結果設置済みぷよと重なるなら回転しない
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
			puyoactive.SetValue(puyo2_y + 1, puyo2_x - 1, puyo2);
			//次の回転パターンの設定
			puyoactive.Rotate_setter(1);
			break;

		case 1:
			//回転パターン
			//R -> BR
			//B
			//Rがpuyo1, Bがpuyo2
			if (puyo2_x <= 0 || puyo2_y <= 0)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}
			if (puyostack.GetValue(puyo1_y,puyo1_x-1) != NONE){     //もし回転した結果設置済みぷよと重なるなら回転しない
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
			puyoactive.SetValue(puyo2_y - 1, puyo2_x - 1, puyo2);

			//次の回転パターンの設定
			puyoactive.Rotate_setter(2);
			break;

		case 2:
			//回転パターン
			//      B
			//BR -> R
			//Bがpuyo1, Rがpuyo2
			if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y <= 0)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}
			if (puyostack.GetValue(puyo2_y-1,puyo2_x) != NONE){      //もし回転した結果設置済みぷよと重なるなら回転しない
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y - 1, puyo1_x + 1, puyo1);
			puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

			//次の回転パターンの設定
			puyoactive.Rotate_setter(3);
			break;

		case 3:
			//回転パターン
			//B
			//R -> RB
			//Bがpuyo1, Rがpuyo2
			if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y >= puyoactive.GetLine() - 1)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}
			if (puyostack.GetValue(puyo2_y,puyo2_x+1) != NONE){     //もし回転した結果設置済みぷよと重なるなら回転しない
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y + 1, puyo1_x + 1, puyo1);
			puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

			//次の回転パターンの設定
			puyoactive.Rotate_setter(0);
			break;

		default:
			break;
		}
	}
	
    void CopyPuyo(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack){
		//puyostackからpuyoactiveへコピー
		for (int y = puyostack.GetLine(); y >= 0; y--)
		{
			for (int x = 0; x < puyostack.GetColumn(); x++)
			{	
				puyoactive.SetValue(y, x,puyostack.GetValue(y,x));
				puyostack.SetValue(y, x, NONE);
				
			}
		}
	}
    // scoreを返す
    int ReturnScore(int chain, int vanishnum) {
        //連鎖ボーナスの配列
        int chain_score_array[19] = {1,   8,   16,  32,  64,  96,  128, 160, 192, 224,
                                    256, 288, 320, 352, 384, 416, 448, 480, 512};
        if (chain >= 1 && chain <= 19) {
            return vanishnum * chain_score_array[chain - 1] * 10;
        }
        return 0;
    }
	//GameOver
	void GameOver(int score) {
        clear();
        char msg1[256], msg2[256], score_str[256];
        sprintf(msg1, "GAME OVER!");
        sprintf(msg2, "Press Q to quit this game.");
		sprintf(score_str, "SCORE: %d",score);
		attrset(COLOR_PAIR(1));
        mvaddstr(0, 0, msg1);
        mvaddstr(1, 0, msg2);
		mvaddstr(2,0,score_str);
        refresh();
        int ch;
        do {
            ch = getch();
        } while (ch != 'Q');
    }

	//Pause
	void Pause(){
		int ch_temp;
        char pause_str[256];
        sprintf(pause_str, "PAUSE");
		attrset(COLOR_PAIR(1));
        mvaddstr(5, COLS-35, pause_str);
        do {
          ch_temp = getch();
        } while (ch_temp != 'p');
        sprintf(pause_str, "     ");
        mvaddstr(5, COLS-35, pause_str);
	}
   
};

//nextpuyo, next_nextpuyoの色
void AttrSetColor(int y, int x, puyocolor color)
{
	switch (color)
		{
		case NONE:
			attrset(COLOR_PAIR(1));
			mvaddch(y, x, ' ');
			break;
		case RED:
			attrset(COLOR_PAIR(2));
			mvaddch(y, x, 'R');
			break;
		case BLUE:
			attrset(COLOR_PAIR(3));
			mvaddch(y, x, 'B');
			break;
		case GREEN:
			attrset(COLOR_PAIR(4));
			mvaddch(y, x, 'G');
			break;
		case YELLOW:
			attrset(COLOR_PAIR(5));
			mvaddch(y, x, 'Y');
			break;
		default:
			break;
		}
}
//表示
void Display(PuyoArrayActive &activepuyo, PuyoArrayStack &stackpuyo, PuyoControl &control, int chain, int score){
	//一時的メモリ保存の場所確保
	puyocolor *puyo_temp = new puyocolor[activepuyo.GetLine() * activepuyo.GetColumn()];
	for(int i = 0; i < activepuyo.GetLine() * activepuyo.GetColumn(); i++){
		puyo_temp[i] = NONE;
	}

	//PuyoArrayActive, PuyoArrayStackを合体
	for (int y = 0; y < activepuyo.GetLine(); y++){
		for(int x = 0; x < activepuyo.GetColumn(); x++ ){
		if(puyo_temp[y*activepuyo.GetColumn()+x] == NONE && activepuyo.GetValue(y,x) != NONE){
			puyo_temp[y*activepuyo.GetColumn()+x] = activepuyo.GetValue(y,x);
		}
		if(puyo_temp[y*activepuyo.GetColumn()+x]== NONE && stackpuyo.GetValue(y,x) != NONE){
			puyo_temp[y*activepuyo.GetColumn()+x] = stackpuyo.GetValue(y,x);
		}
		}
	}
		//落下中ぷよ表示
	for (int y = 0; y < activepuyo.GetLine(); y++)
		{
			for (int x = 0; x < activepuyo.GetColumn(); x++)
			{   
				switch (puyo_temp[y*activepuyo.GetColumn()+x])
				{
				case NONE:
					attrset(COLOR_PAIR(1));
					mvaddch(y, x, '.');
					break;
				case RED:
					attrset(COLOR_PAIR(2));
					mvaddch(y, x, 'R');
					break;
				case BLUE:
					attrset(COLOR_PAIR(3));
					mvaddch(y, x, 'B');
					break;
				case GREEN:
					attrset(COLOR_PAIR(4));
					mvaddch(y, x, 'G');
					break;
				case YELLOW:
					attrset(COLOR_PAIR(5));
					mvaddch(y, x, 'Y');
					break;
				default:
					mvaddch(y, x, '?');
					break;
				}
			}		
		}


		//情報表示
	int count = 0;
	for (int y = 0; y < activepuyo.GetLine(); y++)
		{
			for (int x = 0; x < activepuyo.GetColumn(); x++)
			{
				if (puyo_temp[y*activepuyo.GetColumn() + x] != NONE)
				{
				    count++;
				}
			}
		}
    char msg[256],chain_str[256], score_str[256];
    sprintf(msg, "Field: %d x %d, Puyo number: %03d", activepuyo.GetLine(), activepuyo.GetColumn(), count);
    sprintf(chain_str, "CHAIN: %d", chain);
    sprintf(score_str,"SCORE: %d",score);
	mvaddstr(2, COLS - 35, msg);
    mvaddstr(3, COLS -35, chain_str);
    mvaddstr(4, COLS -35, score_str);

	//nextpuyo, next_nextpuyoの表示
	sprintf(msg, "NEXT");
	mvaddstr(5, COLS - 60, msg);
	sprintf(msg, "####");
	mvaddstr(4, COLS - 53, msg);
	sprintf(msg, "#  #");
	mvaddstr(5, COLS - 53, msg);
	sprintf(msg, "####");
	mvaddstr(6, COLS - 53, msg);
	sprintf(msg, "####");
	mvaddstr(4, COLS - 46, msg);
	sprintf(msg, "#  #");
	mvaddstr(5, COLS - 46, msg);
	sprintf(msg, "####");
	mvaddstr(6, COLS - 46, msg);

	AttrSetColor(5, COLS - 52, control.next_puyo_array[0]);
	AttrSetColor(5, COLS - 51, control.next_puyo_array[1]);
	AttrSetColor(5, COLS - 45, control.next_puyo_array[2]);
	AttrSetColor(5, COLS - 44, control.next_puyo_array[3]);
    refresh();
    delete[] puyo_temp;
};

//ここから実行される
int main(int argc, char **argv){
  
  	PuyoArrayActive active_puyo; PuyoArrayStack stack_puyo; PuyoControl control;
  
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_RED,COLOR_BLACK);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(4,COLOR_GREEN,COLOR_BLACK);
	init_pair(5,COLOR_YELLOW,COLOR_BLACK);
	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);


	//初期化処理
	active_puyo.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	stack_puyo.ChangeSize(LINES/2, COLS/2);
	control.GeneratePuyo(active_puyo);	//最初のぷよ生成

	int delay = 0;
	int waitCount = 20000;
	int puyostate = 0;
    int score = 0;
    int chain = 0;
	int vanishnum;
	bool lock = false;
	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q'){
			break;
		}

		//入力キーごとの処理
		if (lock){
			ch;
		}else{
			switch (ch)
			{
			case KEY_LEFT:
				control.MoveLeft(active_puyo, stack_puyo);
				break;
			case KEY_RIGHT:
				control.MoveRight(active_puyo, stack_puyo);
				break;
				
			case KEY_DOWN:
				control.MoveDown(active_puyo,stack_puyo);
				break;	
				
			case 'z':
				control.Rotate(active_puyo,stack_puyo);
				break;

			case 'p':
				control.Pause();
				break;

			default:
				break;
			}
		}

		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			if (control.LandingPuyo(active_puyo, stack_puyo)){
                lock=true;
				if(active_puyo.CountPuyo()== 0){
                    if((vanishnum = control.VanishPuyo(stack_puyo))!=0){
						control.CopyPuyo(active_puyo,stack_puyo);
						chain++;
                		score += control.ReturnScore(chain, vanishnum);                    
                    }
					else{
						//新しいぷよ生成
						if(stack_puyo.GetValue(0,5)||stack_puyo.GetValue(0,6)){
							control.GameOver(score);
							break;
						}else{
							control.GeneratePuyo(active_puyo);
							chain = 0;
							lock=false;
						}
					}
				}
                //全て消えたとき
				if(active_puyo.CountPuyo() + stack_puyo.CountPuyo() == 0){
					//新しいぷよ生成
					if(stack_puyo.GetValue(0,5)||stack_puyo.GetValue(0,6)){
						control.GameOver(score);
						break;
					}else{
						control.GeneratePuyo(active_puyo);
						chain = 0;
						lock=false;
					}
				}
			}
			//ぷよ下に移動
			else{
				control.MoveDown(active_puyo,stack_puyo);
			}
		}
		delay++;
		//表示
		Display(active_puyo, stack_puyo,control,chain, score);
	}

	//画面をリセット
	endwin();

	return 0;
}
