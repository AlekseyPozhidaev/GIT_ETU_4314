#include <locale.h>
#include <iostream>
#include "screen.h"
#include "shape.h"
// ПРИМЕР ДОБАВКИ: дополнительный фрагмент – полуокружность
class h_circle: public rectangle, public reflectable {

public:
	h_circle(point a, int rd): rectangle(point(a.x-rd, a.y), point(a.x+rd,a.y+rd*0.7+1)) { }
	void draw();
};

void h_circle :: draw()   //Алгоритм Брезенхэма для окружностей
{   // (выдаются два сектора, указываемые значением reflected::vert)
    int x0 = (ne.x + sw.x)/2, y0 = vert ? sw.y : ne.y;
	int radius = (ne.x - sw.x)/2;
	int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while(y >= 0) { // Цикл рисования
	   if(vert) { put_point(x0 + x, y0 + y*0.7); put_point(x0 - x, y0 + y*0.7); }
	   else { put_point(x0 + x, y0 - y*0.7); put_point(x0 - x, y0 - y*0.7); }
       error = 2 * (delta + y) - 1;
       if(delta < 0 && error <= 0) { ++x; delta += 2 * x + 1; continue; }
       error = 2 * (delta - x) - 1;
       if(delta > 0 && error > 0) { --y; delta += 1 - 2 * y; continue; }
       ++x; delta += 2 * (x - y);  --y;
	   }
}

// КОСОЙ КРЕСТ
class olique_cross: public rectangle
{
	public:
		olique_cross(point a, point b): rectangle(a, b) { }
		void draw();
};

void olique_cross:: draw()
{
	put_line(sw.x , sw.y, ne.x, ne.y);
	put_line(nwest().x, nwest().y, seast().x, seast().y);
}

// РОМБ
class rhombus: public shape
{
	protected:
		point c;
		int h_dig;

	public:
		rhombus(point a, int b): c(a), h_dig(b) { }
		
		point north( ) const { return point(c.x, c.y + h_dig); }
  		point south( ) const { return point(c.x, c.y - h_dig); }
  		point east( ) const { return point(c.x + h_dig, c.y); }
  		point west( ) const { return point(c.x - h_dig, c.y); }
  		point neast( ) const { return point(c.x + h_dig, c.y + h_dig); }
  		point seast( ) const { return point(c.x + h_dig, c.y - h_dig); }
  		point nwest( ) const { return point(c.x - h_dig, c.y + h_dig); }
  		point swest( ) const { return point(c.x - h_dig, c.y - h_dig); }

		void draw();
		void move(int a, int b)  {c.x += a, c.y += b;} // НЕ РАБОТАЕТ (не знаю почему)
		void resize(double d) {h_dig*d;}                // Изменение длины линии в (d) раз НЕ РАБОТАЕТ (тоже не понял)

};

void rhombus:: draw()
{
	put_line(west().x, west().y, north().x, north().y);
	put_line(north().x, north().y, east().x, east().y);
	put_line(west().x, west().y, south().x, south().y); 
	put_line(south().x, south().y, east().x, east().y);
}


// РОМБ С КОСЫМ КРЕСТОМ
class cross_rhombus: public rhombus
{
	public:
		cross_rhombus(point(a), int b): rhombus(a, b) { };
		void draw();
};

void cross_rhombus:: draw()
{
	put_line(west().x, west().y, north().x, north().y);
	put_line(north().x, north().y, east().x, east().y);
	put_line(west().x, west().y, south().x, south().y); 
	put_line(south().x, south().y, east().x, east().y);
	put_line(swest().x , swest().y, neast().x, neast().y);
	put_line(nwest().x, nwest().y, seast().x, seast().y);
}

// ПРИМЕР ДОБАВКИ: дополнительная функция присоединения…
void down(shape &p,  const shape &q)
{    point n = q.south( );
     point s = p.north( );
     p.move(n.x - s.x, n.y - s.y - 1); }

void left(shape &p,  const shape &q)
{    point w = q.west( );
     point e = p.east( );
     p.move(e.x - w.x - 1, e.y - w.y); }

void right(shape &p,  const shape &q)
{    point e = q.east( );
     point w = p.west( );
     p.move(w.x - e.x + 1, w.y - e.y); }


// Cборная пользовательская фигура – физиономия
class myshape : public rectangle {      // Моя фигура ЯВЛЯЕТСЯ
     int w, h;			             //        прямоугольником
     line l_eye;    // левый глаз – моя фигура СОДЕРЖИТ линию
     line r_eye;   // правый глаз
     line mouth;  // рот
  public:
     myshape(point, point);
     void draw( );
     void move(int, int);
	 void resize(double r) { rectangle::resize(r); 
     rectangle::move(w*(1-r)*0.5, h*(1-r)*0.5); }
	 void rotate_left( ) { }
	 void rotate_right( ) { }
};


myshape :: myshape(point a, point b): rectangle(a, b),	//Инициализация базового класса
	  w(neast( ).x - swest( ).x + 1), // Инициализация данных
	  h(neast( ).y - swest( ).y + 1), // – строго в порядке объявления!
	  l_eye(point(swest( ).x + 2, swest( ).y + h * 3 / 4), 2),
	  r_eye(point(swest( ).x + w - 4, swest( ).y + h * 3 / 4), 2),
	  mouth(point(swest( ).x + 2, swest( ).y + h / 4), w - 4) { }

	  
void myshape :: draw( )
{
	 rectangle :: draw( );      //Контур лица (глаза и нос рисуются сами!) 
	 int a = (swest( ).x + neast( ).x) / 2;
	 int b = (swest( ).y + neast( ).y) / 2;
	 put_point(point(a, b));   // Нос – существует только на рисунке!
}

void myshape :: move(int a, int b)

{
	 rectangle :: move(a, b);
	 l_eye.move(a, b);  r_eye.move(a, b);
	 mouth.move(a, b);
}

int main( ) 
{   
	setlocale(LC_ALL, "Rus");
	screen_init( );
//== 1. Объявление набора фигур ==
	rectangle hat(point(0, 0), point(14, 5));
	line brim(point(20,9),17);
	myshape face(point(35,20), point(47,28));
	cross_rhombus left_ear(point(40, 40), 5);
	cross_rhombus right_ear(point(47, 40), 5);
	cross_rhombus tie(point(25, 40), 5);
	//h_circle beard(point(40,10), 5);
	shape_refresh( );
	std::cout << "=== Generated... ===\n";
	std::cin.get(); //Смотреть исходный набор
//== 2. Подготовка к сборке ==
	hat.rotate_right( );
	brim.resize(2.0);
	face.resize(1.2);
	//beard.resize(1.2);
	left_ear.resize(0.2);
	right_ear.resize(0.2);
	tie.resize(3.0);
    shape_refresh();
	std::cout << "=== Prepared... ===\n";
	std::cin.get(); //Смотреть результат поворотов/отражений
//== 3. Сборка изображения ==
//	face.move(0, -10); // Лицо – в исходное положение (если нужно!)
	up(brim, face);
	up(hat, brim);
	//down(beard, face);
	right(right_ear, face);
	left(left_ear, face);
	down(tie, face);
	shape_refresh( );
	std::cout << "=== Ready! ===\n";
	std::cin.get();       //Смотреть результат
	screen_destroy( );
	return 0;
}