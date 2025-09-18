#ifndef SIGNAL_H
#define SIGNAL_h

void sigtstp_handler(int sig){
    if(childpid>0)
        kill(childpid,sig);

    rl_replace_line("", 0);
    rl_crlf();
    rl_on_new_line();
    rl_redisplay();
 
}

void sigint_handler(int sig){
    if(childpid>0)
        kill(childpid,sig);

    rl_replace_line("", 0);
    rl_crlf();
    rl_on_new_line();
    rl_redisplay();
}

#endif