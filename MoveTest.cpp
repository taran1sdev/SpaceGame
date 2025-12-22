#include <iostream>


struct Vector2 {
    float x, y;
    
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    
    void add(const Vector2& other) {
        x += other.x;
        y += other.y;
    }
    
    void print() const {
        std::cout << "Position: (" << x << ", " << y << ")\n";
    }
};


struct Bounds {
    Vector2 TopRight, TopLeft, BottomRight, BottomLeft;

    Bounds(Vector2 TopRight = Vector2(5, 5), Vector2 TopLeft = Vector2(-5, 5), Vector2 BottomLeft = Vector2(-5, -5), Vector2 BottomRight = Vector2(5, -5)) :
        TopRight(TopRight), TopLeft(TopLeft), BottomLeft(BottomLeft), BottomRight(BottomRight) {}

    

    bool hitBounds(const Vector2& position){
        if((position.x < TopRight.x && position.y < TopRight.y) && (position.x > BottomLeft.x && position.y > BottomLeft.y)){
            return false;
        }
        else{
            std::cout << "At, or outside Bounds!\n";
            return true;
        }
    }
};


int main() {
    Vector2 position(0, 0); 
    Vector2 velocity(0, 0);
    Bounds bounds; 
    float speed = 1.0f;
    
    std::cout << "Use WASD keys to move. Press 'Q' to quit.\n";
    
    char input;
    while (true) {
        std::cout << "Enter WASD key: ";
        std::cin >> input;
        
        // Reset velocity each frame
        velocity.x = 0;
        velocity.y = 0;
        
        // Map keys to movement
        switch (input) {
            case 'w':
            case 'W':
                velocity.y = speed; // Up
                break;
            case 'a':
            case 'A':
                velocity.x = -speed; // Left
                break;
            case 's':
            case 'S':
                velocity.y = -speed; // Down
                break;
            case 'd':
            case 'D':
                velocity.x = speed; // Right
                break;
            case 'q':
            case 'Q':
                return 0;
            default:
                std::cout << "Invalid input bro\n";
                continue;
        }
        
        // Update position
        position.add(velocity);
        position.print();

        bounds.hitBounds(position);  // check if we hit a bounds (can still go outside it tho)
    }
    
    return 0;
}