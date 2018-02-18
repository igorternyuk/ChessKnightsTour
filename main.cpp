#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <map>

const int board_size {8};
const int cell_size = 60;
bool board[board_size][board_size];

struct Position
{
    int x, y;
};

/*
 * ..2.3..
 * .1...4
 * ...x...
 * .8...5.
 * ..7.6..
*/

const struct
{
    int dx, dy;
} moves[] =
    {
        { -2, 1 },
        { -1, 2 },
        { 1, 2 },
        { 2, 1 },
        { 2, -1 },
        { 1, -2 },
        { -1, -2 },
        { -2, -1 }
    };

const size_t moves_size = sizeof(moves) / sizeof(*moves);

std::vector<Position> solution;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //Chess board
    glBegin(GL_QUADS);
    for(int y{0}; y < board_size; ++y)
    {
        for(int x{0}; x < board_size; ++x)
        {
            if((x + y) % 2 == 0)
                glColor3f(1,1,1);
            else
                glColor3f(0,0,0);
            glVertex2f(x * cell_size, y * cell_size);
            glVertex2f((x + 1) * cell_size, y * cell_size);
            glVertex2f((x + 1) * cell_size, (y + 1) * cell_size);
            glVertex2f(x * cell_size, (y + 1) * cell_size);
        }
    }
    glEnd();
    //Already installed knights
    glColor3f(0,1,0);
    glPointSize(5);
    glBegin(GL_POINTS);
    for(int y{0}; y < board_size; ++y)
    {
        for(int x{0}; x < board_size; ++x)
        {
            if(board[y][x])
            {
                glVertex2f(x * cell_size + cell_size / 2,
                           y * cell_size + cell_size / 2);
            }
        }
    }
    glEnd();
    glPointSize(1);

    //Solution
    glLineWidth(3);
    glColor3f(0,1,0);
    glBegin(GL_LINE_STRIP);
    for(auto it = solution.begin(); it != solution.end(); ++it)
    {
        const int x = it->x * cell_size + cell_size / 2;
        const int y = it->y * cell_size + cell_size / 2;
        glVertex2f(x,y);
    }
    glEnd();
    glutSwapBuffers();
}

bool is_legal_pos(int x, int y)
{
    return x >= 0 && x < board_size && y >= 0 && y < board_size;
}

size_t num_of_possible_moves(int x, int y)
{
    size_t counter{0};
    for(auto m: moves)
    {
        const int nx { x + m.dx };
        const int ny { y + m.dy };
        if(is_legal_pos(nx,ny) && !board[ny][nx])
            ++counter;
    }
    return counter;
}

bool solve(int x, int y)
{
    board[y][x] = true;
    solution.push_back({x,y});
    display();
    if(solution.size() == board_size * board_size)
        return true;
    std::multimap<int /*move index*/,size_t /*number of possible moves*/> sorted_moves;
    for(size_t i{0}; i < moves_size; ++i)
    {
        const int nx { x + moves[i].dx };
        const int ny { y + moves[i].dy };
        if(is_legal_pos(nx, ny) && !board[ny][nx])
        {
            auto num = num_of_possible_moves(nx, ny);
            sorted_moves.insert({num, i});
        }
    }

    for(auto m: sorted_moves)
    {
        const int nx { x + moves[m.second].dx };
        const int ny { y + moves[m.second].dy };
        if(solve(nx,ny))
            return true;
    }

    board[y][x] = false;
    solution.pop_back();
    return false;
}

void timer(int = 0)
{
    for(int y{0}; y < board_size; ++y)
        for(int x{0}; x < board_size; ++x)
            board[y][x] = false;
    solution.clear();
    solve(0,0);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(480, 480);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Chess knight's tour!");
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,480,480,0,-1,1);
    glutDisplayFunc(display);
    glutTimerFunc(300, timer, 0);
    glutMainLoop();

    return 0;
}
