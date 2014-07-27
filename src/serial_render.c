#include <GLFW/glfw3.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>


/*
 * The serialport_init function is taken from the 'arduino-serial' library
 * located at https://github.com/todbot/arduino-serial.git
 * Courtesy of Tod E. Kurt
 */

int serialport_init(const char* serialport, int baud)
{
    struct termios toptions;
    int fd;

    //fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    fd = open(serialport, O_RDWR | O_NONBLOCK );

    if (fd == -1)  {
        perror("serialport_init: Unable to open port ");
        return -1;
    }

    //int iflags = TIOCM_DTR;
    //ioctl(fd, TIOCMBIS, &iflags);     // turn on DTR
    //ioctl(fd, TIOCMBIC, &iflags);    // turn off DTR

    if (tcgetattr(fd, &toptions) < 0) {
        perror("serialport_init: Couldn't get term attributes");
        return -1;
    }
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
#ifdef B14400
    case 14400:  brate=B14400;  break;
#endif
    case 19200:  brate=B19200;  break;
#ifdef B28800
    case 28800:  brate=B28800;  break;
#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    //toptions.c_cflag &= ~HUPCL; // disable hang-up-on-close to avoid reset

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 0;
    //toptions.c_cc[VTIME] = 20;

    tcsetattr(fd, TCSANOW, &toptions);
    if( tcsetattr(fd, TCSAFLUSH, &toptions) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Second arg should be serial device\n");
        return 1;
    }

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int w = 640;
    int h = 480;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(w, h, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    int USB = serialport_init(argv[1], 19200);
    unsigned char val = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);
        int nbars = 64;
        /* Render here */
        for(int i = 0; i < nbars; i++) {
            read(USB, &val, 1);
            double vh = ((val / 255.0) * 2) - 1;
            double bar_w = 1.0 / (float)nbars;
            double l = i * bar_w;
            double r = (i + 1) * bar_w;
            l = (l * 2) - 1;
            r = (r * 2) - 1;

            glBegin(GL_TRIANGLES);
            glColor3f(0.0, 0.3, 0.7);
            glVertex2f(l, -1);
            glVertex2f(r, -1);
            glVertex2f(r, vh);
            glVertex2f(r, vh);
            glVertex2f(l, vh);
            glVertex2f(l, -1);
            glEnd();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
