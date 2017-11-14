#include <allegro.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

struct flappy {
       BITMAP *flap; 
       int pos_x;
       int pos_y;     
       int jump;  
       int allow;  
       int angle;
       int cool;
       int restrict;
       int score;
       int collided;
       double interval;  
       double drop;   
       
       int Hit_R;
       int Hit_L;
       int Hit_B;
       int Hit_T;
}bird;

struct pipe1 {
       BITMAP *pipelarge;
       BITMAP *pipesmall;
       int pos_x;
       int pos_y;   
       int pos_y_2;  
       int visible; 
       int variation;
       int centre;
       
       int Hit_R1;
       int Hit_L1;
       int Hit_T1;
       int Hit_B1;
       
       int Hit_R2;
       int Hit_L2;
       int Hit_T2;
       int Hit_B2;
}pipe1;

struct pipe2 {
       BITMAP *pipelarge;
       BITMAP *pipesmall;
       int pos_x;
       int pos_y; 
       int pos_y_2;   
       int visible;    
       int variation;
       int centre;
       
       int Hit_R1;
       int Hit_L1;
       int Hit_T1;
       int Hit_B1;
       
       int Hit_R2;
       int Hit_L2;
       int Hit_T2;
       int Hit_B2;
}pipe2;

volatile long speed_counter = 0; //A long integer which will store the value of the
								 //speed counter.

void increment_speed_counter() //A function to increment the speed counter
{
	speed_counter++; // This will just increment the speed counter by one. :)
}
END_OF_FUNCTION(increment_speed_counter); //Make sure you tell it that it's the end of the
										  //function									  
void initFlap(struct flappy &bird); 
void initPipe(struct pipe1 &pipe1, struct pipe2 &pipe2);
void flapMove(struct flappy &bird);
void flapBorder(struct flappy &bird);
void flapHitbox(struct flappy &bird);
void drawHitbox(BITMAP *buffer,struct flappy &bird, struct pipe1 &pipe1, struct pipe2 &pipe2);
void pipeMovement(struct pipe1 &pipe1, struct pipe2 &pipe2);
void setPipeHitbox(struct pipe1 &pipe1, struct pipe2 &pipe2);
void checkCollision(int *Collision, struct flappy &bird, struct pipe1 &pipe1, struct pipe2 &pipe2);
int groundHit(struct flappy &bird); 

/* Set up the general main function, and init allegro. */
int main(int argc, char *argv[])
{
	allegro_init(); // Initialize Allegro
	install_keyboard(); // Initialize keyboard routines
	
	/* You need to tell allegro to install the timer routines in order to use timers. */
	install_timer(); // Initialize the timer routines
	
	LOCK_VARIABLE(speed_counter); //Used to set the timer - which regulates the game's
	LOCK_FUNCTION(increment_speed_counter);//speed.
		
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(60)); //Set our BPS

	set_color_depth(desktop_color_depth()); // Set the color depth
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 960,540,0,0); // Change our graphics mode to 640x480 
	
	srand(time(0));
	initFlap(bird);
	initPipe(pipe1, pipe2);
	BITMAP *buffer; //Declare a BITMAP called buffer.
	BITMAP *background;
	int Collision[4] = {TRUE, TRUE, TRUE, TRUE};
	int i;
	int highscore;
	FILE *record;
	
	background = load_bitmap("background.bmp", NULL);
	
	buffer = create_bitmap(960,540); //Create an empty bitmap.
	
	while(!key[KEY_ESC]) //If the user hits escape, quit the program
	{

		while(speed_counter > 0)
		{
            groundHit(bird);
            flapBorder(bird);
            flapMove(bird);
            flapHitbox(bird);
            
            if(bird.collided == FALSE)
            {
                pipeMovement(pipe1, pipe2);
            }
            setPipeHitbox(pipe1, pipe2);
                
            checkCollision(Collision, bird, pipe1, pipe2);
            
            if(((bird.pos_x + 24) == pipe1.centre || (bird.pos_x + 24) == pipe2.centre) && bird.collided == FALSE)
            {
                bird.score++;               
            }
            
            if(bird.collided == TRUE && bird.pos_y == 512)
            {
                record = fopen("highscore.txt","r"); 
                fscanf(record, "%d", &highscore); 
                fclose(record);             
                
                if(bird.score > highscore)
                {
                    record = fopen("highscore.txt","w");                      
                    fprintf(record, "%d", bird.score);           
                    fclose(record);
                    allegro_message("New Highscore!!!\nHighScore: %d\nScore: %d", bird.score, bird.score);
                }
                else
                {
                    allegro_message("HighScore: %d\nScore: %d", highscore, bird.score);
                }                 
                return 0;                 
            }
			speed_counter --;	
		} // This is the closing bracket for the (speed_counter > 0) test
		
		for(i = 0;i < 4;i++)
		{
    		if(Collision[i] == TRUE && bird.collided == FALSE)
        	{
                bird.drop = 1.7;                
                bird.angle = 210;
                bird.collided = TRUE;                       
            }
        }
        
        blit(background, buffer, 0, 0, 0, 0, screen->w, screen->h);	
        if(bird.collided == FALSE)
        {
            rotate_sprite(buffer, bird.flap, bird.pos_x, bird.pos_y, itofix(bird.angle));
        }
        
		draw_sprite(buffer, pipe1.pipelarge, pipe1.pos_x, pipe1.pos_y);
		draw_sprite(buffer, pipe2.pipelarge, pipe2.pos_x, pipe2.pos_y);
		draw_sprite(buffer, pipe1.pipesmall, pipe1.pos_x, pipe1.pos_y_2);
		draw_sprite(buffer, pipe2.pipesmall, pipe2.pos_x, pipe2.pos_y_2);
		if(bird.collided == TRUE)
        {
            rotate_sprite(buffer, bird.flap, bird.pos_x, bird.pos_y, itofix(bird.angle));
        }
        
        if(key[KEY_O])
        {
    		line(buffer, pipe1.centre, 0, pipe1.centre, 540, makecol(255,0,0));
    		line(buffer, pipe2.centre, 0, pipe2.centre, 540, makecol(255,0,0));
    		
    		drawHitbox(buffer, bird, pipe1, pipe2);	
        }
		textprintf_ex(buffer, font, 0,0, makecol(255,255,255), -1, "Score: %d", bird.score);
        
		blit(buffer, screen, 0,0,0,0,960,540); //Draw the buffer to the screen
		clear_bitmap(buffer); // Clear the contents of the buffer bitmap 
		     																																																						release_screen();// Release it
	} // This is the closing bracket for the (key[KEY_ESC]) test

	/* Do all the destroying, finishing parts of the program. */
	destroy_bitmap(bird.flap); //Release the bitmap data
	destroy_bitmap(pipe1.pipelarge); //Release the bitmap data
	destroy_bitmap(pipe2.pipelarge); //Release the bitmap data
	destroy_bitmap(pipe1.pipesmall); //Release the bitmap data
	destroy_bitmap(pipe2.pipesmall); //Release the bitmap data
	destroy_bitmap(background); //Release the bitmap data 
	destroy_bitmap(buffer); //Release the bitmap data 
	return 0; // Exit with no errors
}
END_OF_MAIN() 

void initFlap(struct flappy &bird)
{
     bird.flap = load_bitmap("flappy.bmp", NULL); // Load our picture    
     bird.pos_x = 300;
     bird.pos_y = 270; 
     bird.angle = 210;
     bird.interval = 0;
     bird.drop = 0;
     bird.cool = 0;
     bird.score = 0;
     bird.collided = FALSE;
     bird.jump = TRUE;
     bird.restrict = FALSE;
     
     bird.Hit_R = bird.pos_x + 40;
     bird.Hit_L = bird.pos_x + 3; 
     bird.Hit_T = bird.pos_y;
     bird.Hit_B = bird.pos_y + 36;
}

void initPipe(struct pipe1 &pipe1, struct pipe2 &pipe2)
{
    pipe1.variation = rand()%3+1; 
    switch(pipe1.variation)
    {
        case 1:
             pipe1.pipelarge = load_bitmap("pipe.bmp", NULL);  
             pipe1.pipesmall = load_bitmap("pipe inverted.bmp", NULL);     
             break; 
        case 2:
             pipe1.pipelarge = load_bitmap("pipe_large.bmp", NULL);  
             pipe1.pipesmall = load_bitmap("pipe_small.bmp", NULL);     
             break;    
        case 3:
             pipe1.pipelarge = load_bitmap("pipe_large inverted.bmp", NULL);  
             pipe1.pipesmall = load_bitmap("pipe_small inverted.bmp", NULL);     
             break;                      
    }
    
    pipe1.pos_x = 960;
    pipe1.centre = pipe1.pos_x + 45;
    pipe1.visible = TRUE;
    
    switch(pipe1.variation)
    {
        case 1:
             pipe1.pos_y = 0;  
             pipe1.pos_y_2 = 540 - 186;  
             break; 
        case 2:  
             pipe1.pos_y = 0;  
             pipe1.pos_y_2 = 540-90;  
             break;            
        case 3:
             pipe1.pos_y = 540-282;  
             pipe1.pos_y_2 = 0;  
             break;                      
    }
    
    switch(pipe1.variation)
    {
        case 1:
             pipe1.Hit_L1 = pipe1.pos_x;
             pipe1.Hit_R1 = pipe1.pos_x + 90;
             pipe1.Hit_T1 = pipe1.pos_y;
             pipe1.Hit_B1 = pipe1.pos_y + 186;
            
             pipe1.Hit_L2 = pipe1.pos_x;
             pipe1.Hit_R2 = pipe1.pos_x + 90;
             pipe1.Hit_T2 = pipe1.pos_y_2;
             pipe1.Hit_B2 = pipe1.pos_y_2 + 186;  
             break; 
        case 2:              
        case 3:
             pipe1.Hit_L1 = pipe1.pos_x;
             pipe1.Hit_R1 = pipe1.pos_x + 90;
             pipe1.Hit_T1 = pipe1.pos_y;
             pipe1.Hit_B1 = pipe1.pos_y + 282;
            
             pipe1.Hit_L2 = pipe1.pos_x;
             pipe1.Hit_R2 = pipe1.pos_x + 90;
             pipe1.Hit_T2 = pipe1.pos_y_2;
             pipe1.Hit_B2 = pipe1.pos_y_2 + 90;  
             break;                      
    }
    
    pipe2.variation = rand()%3+1; 
    
    switch(pipe2.variation)
    {
        case 1:
             pipe2.pipelarge = load_bitmap("pipe.bmp", NULL);  
             pipe2.pipesmall = load_bitmap("pipe inverted.bmp", NULL);     
             break; 
        case 2:
             pipe2.pipelarge = load_bitmap("pipe_large.bmp", NULL);  
             pipe2.pipesmall = load_bitmap("pipe_small.bmp", NULL);     
             break;    
        case 3:
             pipe2.pipelarge = load_bitmap("pipe_large inverted.bmp", NULL);  
             pipe2.pipesmall = load_bitmap("pipe_small inverted.bmp", NULL);     
             break;                      
    }
    pipe2.pos_x = 960;
    
    switch(pipe2.variation)
    {
        case 1:
             pipe2.pos_y = 0;  
             pipe2.pos_y_2 = 540 - 186;  
             break; 
        case 2:  
             pipe2.pos_y = 0;  
             pipe2.pos_y_2 = 540-90;  
             break;            
        case 3:
             pipe2.pos_y = 540-282;  
             pipe2.pos_y_2 = 0;  
             break;                      
    }
    
    pipe2.centre = pipe2.pos_x + 45;
    pipe2.visible = FALSE;
    
    switch(pipe2.variation)
    {
        case 1:
             pipe2.Hit_L1 = pipe2.pos_x;
             pipe2.Hit_R1 = pipe2.pos_x + 90;
             pipe2.Hit_T1 = pipe2.pos_y;
             pipe2.Hit_B1 = pipe2.pos_y + 186;
            
             pipe2.Hit_L2 = pipe2.pos_x;
             pipe2.Hit_R2 = pipe2.pos_x + 90;
             pipe2.Hit_T2 = pipe2.pos_y_2;
             pipe2.Hit_B2 = pipe2.pos_y_2 + 186;  
             break; 
        case 2:              
        case 3:
             pipe2.Hit_L1 = pipe2.pos_x;
             pipe2.Hit_R1 = pipe2.pos_x + 90;
             pipe2.Hit_T1 = pipe2.pos_y;
             pipe2.Hit_B1 = pipe2.pos_y + 282;
            
             pipe2.Hit_L2 = pipe2.pos_x;
             pipe2.Hit_R2 = pipe2.pos_x + 90;
             pipe2.Hit_T2 = pipe2.pos_y_2;
             pipe2.Hit_B2 = pipe2.pos_y_2 + 90;  
             break;                      
    }
    
}

void flapHitbox(struct flappy &bird)
{
    bird.Hit_R = bird.pos_x + 40;
    bird.Hit_L = bird.pos_x + 3; 
    bird.Hit_T = bird.pos_y;
    bird.Hit_B = bird.pos_y + 36;     
}

void drawHitbox(BITMAP *buffer, struct flappy &bird, struct pipe1 &pipe1, struct pipe2 &pipe2)
{
    line(buffer, bird.Hit_L, bird.Hit_T, bird.Hit_L, bird.Hit_B, makecol(255,0,0));
	line(buffer, bird.Hit_R, bird.Hit_T, bird.Hit_R, bird.Hit_B, makecol(255,0,0));
	line(buffer, bird.Hit_L, bird.Hit_T, bird.Hit_R, bird.Hit_T, makecol(255,0,0));
	line(buffer, bird.Hit_L, bird.Hit_B, bird.Hit_R, bird.Hit_B, makecol(255,0,0));
	
	line(buffer, pipe1.Hit_L1, pipe1.Hit_T1, pipe1.Hit_L1, pipe1.Hit_B1, makecol(255,0,0));
	line(buffer, pipe1.Hit_R1, pipe1.Hit_T1, pipe1.Hit_R1, pipe1.Hit_B1, makecol(255,0,0));
	line(buffer, pipe1.Hit_L1, pipe1.Hit_T1, pipe1.Hit_R1, pipe1.Hit_T1, makecol(255,0,0));
	line(buffer, pipe1.Hit_L1, pipe1.Hit_B1, pipe1.Hit_R1, pipe1.Hit_B1, makecol(255,0,0));
	
	line(buffer, pipe1.Hit_L2, pipe1.Hit_T2, pipe1.Hit_L2, pipe1.Hit_B2, makecol(255,0,0));
	line(buffer, pipe1.Hit_R2, pipe1.Hit_T2, pipe1.Hit_R2, pipe1.Hit_B2, makecol(255,0,0));
	line(buffer, pipe1.Hit_L2, pipe1.Hit_T2, pipe1.Hit_R2, pipe1.Hit_T2, makecol(255,0,0));
	line(buffer, pipe1.Hit_L2, pipe1.Hit_B2, pipe1.Hit_R2, pipe1.Hit_B2, makecol(255,0,0));
	
	line(buffer, pipe2.Hit_L1, pipe2.Hit_T1, pipe2.Hit_L1, pipe2.Hit_B1, makecol(255,0,0));
	line(buffer, pipe2.Hit_R1, pipe2.Hit_T1, pipe2.Hit_R1, pipe2.Hit_B1, makecol(255,0,0));
	line(buffer, pipe2.Hit_L1, pipe2.Hit_T1, pipe2.Hit_R1, pipe2.Hit_T1, makecol(255,0,0));
	line(buffer, pipe2.Hit_L1, pipe2.Hit_B1, pipe2.Hit_R1, pipe2.Hit_B1, makecol(255,0,0));
	
	line(buffer, pipe2.Hit_L2, pipe2.Hit_T2, pipe2.Hit_L2, pipe2.Hit_B2, makecol(255,0,0));
	line(buffer, pipe2.Hit_R2, pipe2.Hit_T2, pipe2.Hit_R2, pipe2.Hit_B2, makecol(255,0,0));
	line(buffer, pipe2.Hit_L2, pipe2.Hit_T2, pipe2.Hit_R2, pipe2.Hit_T2, makecol(255,0,0));
	line(buffer, pipe2.Hit_L2, pipe2.Hit_B2, pipe2.Hit_R2, pipe2.Hit_B2, makecol(255,0,0));              
}

void flapMove(struct flappy &bird)
{
     if(key[KEY_SPACE] && bird.jump == FALSE && bird.cool == 0 && bird.restrict == FALSE && bird.collided == FALSE)// If the user hits the right key, change the picture's X coordinate
	 {
	 	 bird.jump = TRUE; 
	 	 bird.angle = 210;
	 	 bird.drop = 0;
  	 }
     
     if(key[KEY_SPACE])
     {
         bird.restrict = TRUE;                                        
     }
     else
     {
         bird.restrict = FALSE;    
     }
     
     if(bird.jump == FALSE)
     {
         if(bird.collided == TRUE)
         {
              bird.pos_y = bird.pos_y + int((pow(bird.drop, 2)) + 1);   
              bird.drop+=0.5;                  
         }
         else
         {
             bird.pos_y = bird.pos_y + int(9.8 * (pow(bird.drop, 2)));  //gravity
             bird.drop+=0.03;   
         }                  
     }   
      
     if(bird.jump == TRUE)
     {
         bird.pos_y = bird.pos_y + int(1.3 * (pow((bird.interval - 1.6), 2)) - 7); 
         bird.interval+=0.3;
         if(bird.interval >= 3.8)
         {
             bird.interval = 0;
             bird.jump = FALSE;                
         }                             
     }
     
     if(bird.collided == TRUE && !(bird.pos_y == 512))
     {
         bird.pos_x += rand()%5 - 2;                 
     }
     bird.drop+=0.02; 
     
     if(bird.drop >= 28.0)
     {
         bird.drop = 28;             
     }
     
     if(groundHit(bird) != 1)
     {
         if(bird.drop >= 1.1)
         {
             bird.angle+=3;
         }
         else
         {
             bird.angle++;
         }      
     }    
     
     if(bird.angle >= 256)
     {
         bird.angle = 0;              
     }
     if(bird.angle >= 64 && bird.angle < 210)
     {  
         bird.angle = 64;
     }
}    

void flapBorder(struct flappy &bird)
{
     if(bird.pos_y <= 0)
     {
         bird.jump = FALSE;
         bird.cool = 10;
         bird.interval = 4.9;                        
     }    
     
     if(bird.cool != 0)
     {
         bird.cool--;             
     }
}

int groundHit(struct flappy &bird)
{
     if(bird.pos_y >= 512)
     {  
         bird.pos_y = 512;  
         bird.collided = TRUE;
         return 1;            
     }    
     return 0;
}

void pipeMovement(struct pipe1 &pipe1, struct pipe2 &pipe2)
{
    if(pipe1.visible == TRUE)
    {                
        pipe1.pos_x-=4;
        if(pipe1.pos_x == 364)
        {
            pipe2.visible = TRUE;                              
        }
        if(pipe1.pos_x <= -90)
        {
            pipe1.visible = FALSE;   
            pipe1.variation = rand()%3+1; 
            switch(pipe1.variation)
            {
                case 1:
                     pipe1.pos_y = 0;  
                     pipe1.pos_y_2 = 540 - 186;  
                     break; 
                case 2:  
                     pipe1.pos_y = 0;  
                     pipe1.pos_y_2 = 540-90;  
                     break;            
                case 3:
                     pipe1.pos_y = 540-282;  
                     pipe1.pos_y_2 = 0;  
                     break;                      
            }
            switch(pipe1.variation)
            {
                case 1:
                     pipe1.pipelarge = load_bitmap("pipe.bmp", NULL);  
                     pipe1.pipesmall = load_bitmap("pipe inverted.bmp", NULL);     
                     break; 
                case 2:
                     pipe1.pipelarge = load_bitmap("pipe_large.bmp", NULL);  
                     pipe1.pipesmall = load_bitmap("pipe_small.bmp", NULL);     
                     break;    
                case 3:
                     pipe1.pipelarge = load_bitmap("pipe_large inverted.bmp", NULL);  
                     pipe1.pipesmall = load_bitmap("pipe_small inverted.bmp", NULL);     
                     break;                      
            }
            pipe1.pos_x = 960;             
        }
    } 
    
    if(pipe2.visible == TRUE)
    {                
        pipe2.pos_x-=4;
        if(pipe2.pos_x == 364)
        {
            pipe1.visible = TRUE;                              
        }
        if(pipe2.pos_x <= -90)
        {
            pipe2.visible = FALSE; 
            pipe2.variation = rand()%3+1; 
            switch(pipe2.variation)
            {
                case 1:
                     pipe2.pos_y = 0;  
                     pipe2.pos_y_2 = 540 - 186;  
                     break; 
                case 2:  
                     pipe2.pos_y = 0;  
                     pipe2.pos_y_2 = 540-90;  
                     break;            
                case 3:
                     pipe2.pos_y = 540-282;  
                     pipe2.pos_y_2 = 0;  
                     break;                      
            }
            switch(pipe2.variation)
            {
                case 1:
                     pipe2.pipelarge = load_bitmap("pipe.bmp", NULL);  
                     pipe2.pipesmall = load_bitmap("pipe inverted.bmp", NULL);     
                     break; 
                case 2:
                     pipe2.pipelarge = load_bitmap("pipe_large.bmp", NULL);  
                     pipe2.pipesmall = load_bitmap("pipe_small.bmp", NULL);     
                     break;    
                case 3:
                     pipe2.pipelarge = load_bitmap("pipe_large inverted.bmp", NULL);  
                     pipe2.pipesmall = load_bitmap("pipe_small inverted.bmp", NULL);     
                     break;                      
            }    
            pipe2.pos_x = 960;          
        }
    }   
    
    pipe1.centre = pipe1.pos_x + 44;
    pipe2.centre = pipe2.pos_x + 44;                   
}

void setPipeHitbox(struct pipe1 &pipe1, struct pipe2 &pipe2)
{
     
    switch(pipe1.variation)
    {
        case 1:
             pipe1.Hit_L1 = pipe1.pos_x;
             pipe1.Hit_R1 = pipe1.pos_x + 90;
             pipe1.Hit_T1 = pipe1.pos_y;
             pipe1.Hit_B1 = pipe1.pos_y + 186;
            
             pipe1.Hit_L2 = pipe1.pos_x;
             pipe1.Hit_R2 = pipe1.pos_x + 90;
             pipe1.Hit_T2 = pipe1.pos_y_2;
             pipe1.Hit_B2 = pipe1.pos_y_2 + 186;  
             break; 
        case 2:              
        case 3:
             pipe1.Hit_L1 = pipe1.pos_x;
             pipe1.Hit_R1 = pipe1.pos_x + 90;
             pipe1.Hit_T1 = pipe1.pos_y;
             pipe1.Hit_B1 = pipe1.pos_y + 282;
            
             pipe1.Hit_L2 = pipe1.pos_x;
             pipe1.Hit_R2 = pipe1.pos_x + 90;
             pipe1.Hit_T2 = pipe1.pos_y_2;
             pipe1.Hit_B2 = pipe1.pos_y_2 + 90;  
             break;                      
    }
    
    switch(pipe2.variation)
    {
        case 1:
             pipe2.Hit_L1 = pipe2.pos_x;
             pipe2.Hit_R1 = pipe2.pos_x + 90;
             pipe2.Hit_T1 = pipe2.pos_y;
             pipe2.Hit_B1 = pipe2.pos_y + 186;
            
             pipe2.Hit_L2 = pipe2.pos_x;
             pipe2.Hit_R2 = pipe2.pos_x + 90;
             pipe2.Hit_T2 = pipe2.pos_y_2;
             pipe2.Hit_B2 = pipe2.pos_y_2 + 186;  
             break; 
        case 2:              
        case 3:
             pipe2.Hit_L1 = pipe2.pos_x;
             pipe2.Hit_R1 = pipe2.pos_x + 90;
             pipe2.Hit_T1 = pipe2.pos_y;
             pipe2.Hit_B1 = pipe2.pos_y + 282;
            
             pipe2.Hit_L2 = pipe2.pos_x;
             pipe2.Hit_R2 = pipe2.pos_x + 90;
             pipe2.Hit_T2 = pipe2.pos_y_2;
             pipe2.Hit_B2 = pipe2.pos_y_2 + 90;  
             break;                      
    }
    
}

void checkCollision(int *Collision, struct flappy &bird, struct pipe1 &pipe1, struct pipe2 &pipe2)
{
    int i;
    for(i = 0;i < 4;i++)
    {
        Collision[i] = TRUE;      
    }         
               
    if(bird.Hit_B < pipe2.Hit_T1)
	{
		Collision[0] = FALSE;
	}
	else if(bird.Hit_T > pipe2.Hit_B1)
	{
		Collision[0] = FALSE;
	}
	else if(bird.Hit_R < pipe2.Hit_L1)
	{
		Collision[0] = FALSE;
	}
	else if(bird.Hit_L > pipe2.Hit_R1)
	{
		Collision[0] = FALSE;
	}         
               
    if(bird.Hit_B < pipe1.Hit_T1)
	{
		Collision[1] = FALSE;
	}
	else if(bird.Hit_T > pipe1.Hit_B1)
	{
		Collision[1] = FALSE;
	}
	else if(bird.Hit_R < pipe1.Hit_L1)
	{
		Collision[1] = FALSE;
	}
	else if(bird.Hit_L > pipe1.Hit_R1)
	{
		Collision[1] = FALSE;
	}  
	
	if(bird.Hit_B < pipe2.Hit_T2)
	{
		Collision[2] = FALSE;
	}
	else if(bird.Hit_T > pipe2.Hit_B2)
	{
		Collision[2] = FALSE;
	}
	else if(bird.Hit_R < pipe2.Hit_L2)
	{
		Collision[2] = FALSE;
	}
	else if(bird.Hit_L > pipe2.Hit_R2)
	{
		Collision[2] = FALSE;
	}         
               
    if(bird.Hit_B < pipe1.Hit_T2)
	{
		Collision[3] = FALSE;
	}
	else if(bird.Hit_T > pipe1.Hit_B2)
	{
		Collision[3] = FALSE;
	}
	else if(bird.Hit_R < pipe1.Hit_L2)
	{
		Collision[3] = FALSE;
	}
	else if(bird.Hit_L > pipe1.Hit_R2)
	{
		Collision[3] = FALSE;
	}   
}
