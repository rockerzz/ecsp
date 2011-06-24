#include <engine/engine.h>

int main(int argc, char *argv[])
{
    ecsp::Engine engine( argc, argv, "server" );
    return engine.run();
}

