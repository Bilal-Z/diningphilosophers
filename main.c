#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h>
#include <gtk/gtk.h>

sem_t chopsticks[5];
sem_t pick;
int exitStatus;
int meals[5];
GtkBuilder *builder;
GtkWidget *root;
GtkWidget *stick[5];
GtkWidget *hand[5];
GtkWidget *think[5];
GtkWidget *stomach[5];
GtkWidget *bowl[5];
GtkWidget *labels[5];
GtkCssProvider *styling;
GFile *file;

void checkExitStatus(){
    if(exitStatus){
        pthread_exit(0);
    }
}

void update(int i){
    gchar *show;
    show = g_strdup_printf("%d", meals[i]);
    gtk_label_set_text(GTK_LABEL(labels[i]), show);
    g_free(show);
}

void* dining(void *ptr){
    int n = *((int*)ptr), left, right, hungry = 0;
    update(n - 1);
    gtk_widget_hide(hand[n - 1]);
    gtk_widget_hide(bowl[n - 1]);
    gtk_widget_hide(stomach[n - 1]);
    gtk_widget_show(think[n - 1]);

    while(1){
        if(!hungry){
            checkExitStatus();
            printf("philosoper %d is thinking\n", n);
            checkExitStatus();
            gtk_widget_hide(bowl[n - 1]);
            gtk_widget_hide(stomach[n - 1]);
            gtk_widget_show(think[n - 1]);

            sleep((rand() % 6) + 2);
            checkExitStatus();
            printf("philosoper %d is hungry\n", n);
            checkExitStatus();
            gtk_widget_hide(bowl[n - 1]);
            gtk_widget_show(stomach[n - 1]);
            gtk_widget_hide(think[n - 1]);

            hungry = 1;
        }
        
        sem_getvalue(&chopsticks[n - 1], &right);
        sem_getvalue(&chopsticks[n % 5], &left);
        if(left && right){
            checkExitStatus();
            sem_wait(&pick);
            sem_wait(&chopsticks[n -1]);
            sem_wait(&chopsticks[n % 5]);
            checkExitStatus();
            gtk_widget_hide(stick[n - 1]);
            gtk_widget_hide(stick[n % 5]);
            gtk_widget_show(hand[n - 1]);

            printf("philosoper %d is taking chopstick %d and %d\n", n, n, (n + 1) % 5);
            printf("philosoper %d is eating\n", n);
            checkExitStatus();
            meals[n-1] += 1;
            checkExitStatus();
            update(n - 1);
            gtk_widget_show(bowl[n - 1]);
            gtk_widget_hide(stomach[n - 1]);
            gtk_widget_hide(think[n - 1]);

            
            hungry = 0;

            sem_post(&pick);

            checkExitStatus();
            sleep(2);

            printf("philosoper %d is releasing chopstick %d and %d\n", n, n, (n + 1) % 5);
            checkExitStatus();
            gtk_widget_show(stick[n - 1]);
            gtk_widget_show(stick[n % 5]);
            gtk_widget_hide(hand[n - 1]);

            sem_post(&chopsticks[n - 1]);
            sem_post(&chopsticks[n % 5]);

        }

        checkExitStatus();
    }
}

void quit_program(){
    exitStatus = 1;
    gtk_main_quit();
}

int main(int argc, char *argv[]){
    
    int philosophers[] = {1, 2, 3, 4, 5};
    pthread_t threads[5];
    int i;
    exitStatus = 0;
    for(i = 0; i < 5; i++){
        meals[i] = 0;
        sem_init(&chopsticks[i], 0, 1);
    }
    sem_init(&pick, 0, 1);

    //++++++++++++GTK GUI INITIALIZE+++++++++++++++++++++++++++++++++++++++++++
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("gui.glade");
    root = GTK_WIDGET(gtk_builder_get_object(builder, "root"));
    file = g_file_new_for_path("style.css");
    styling = gtk_css_provider_new();
    gtk_css_provider_load_from_file(styling, file, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(root), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);

    stick[0] = GTK_WIDGET(gtk_builder_get_object(builder, "stick1"));
    stick[1] = GTK_WIDGET(gtk_builder_get_object(builder, "stick2"));
    stick[2] = GTK_WIDGET(gtk_builder_get_object(builder, "stick3"));
    stick[3] = GTK_WIDGET(gtk_builder_get_object(builder, "stick4"));
    stick[4] = GTK_WIDGET(gtk_builder_get_object(builder, "stick5"));
    
    hand[0] = GTK_WIDGET(gtk_builder_get_object(builder, "hand1"));
    hand[1] = GTK_WIDGET(gtk_builder_get_object(builder, "hand2"));
    hand[2] = GTK_WIDGET(gtk_builder_get_object(builder, "hand3"));
    hand[3] = GTK_WIDGET(gtk_builder_get_object(builder, "hand4"));
    hand[4] = GTK_WIDGET(gtk_builder_get_object(builder, "hand5"));

    think[0] = GTK_WIDGET(gtk_builder_get_object(builder, "s1"));
    think[1] = GTK_WIDGET(gtk_builder_get_object(builder, "s2"));
    think[2] = GTK_WIDGET(gtk_builder_get_object(builder, "s3"));
    think[3] = GTK_WIDGET(gtk_builder_get_object(builder, "s4"));
    think[4] = GTK_WIDGET(gtk_builder_get_object(builder, "s5"));

    stomach[0] = GTK_WIDGET(gtk_builder_get_object(builder, "s6"));
    stomach[1] = GTK_WIDGET(gtk_builder_get_object(builder, "s7"));
    stomach[2] = GTK_WIDGET(gtk_builder_get_object(builder, "s8"));
    stomach[3] = GTK_WIDGET(gtk_builder_get_object(builder, "s9"));
    stomach[4] = GTK_WIDGET(gtk_builder_get_object(builder, "s10"));

    bowl[0] = GTK_WIDGET(gtk_builder_get_object(builder, "s11"));
    bowl[1] = GTK_WIDGET(gtk_builder_get_object(builder, "s12"));
    bowl[2] = GTK_WIDGET(gtk_builder_get_object(builder, "s13"));
    bowl[3] = GTK_WIDGET(gtk_builder_get_object(builder, "s14"));
    bowl[4] = GTK_WIDGET(gtk_builder_get_object(builder, "s15"));

    labels[0] = GTK_WIDGET(gtk_builder_get_object(builder, "l1"));
    labels[1] = GTK_WIDGET(gtk_builder_get_object(builder, "l2"));
    labels[2] = GTK_WIDGET(gtk_builder_get_object(builder, "l3"));
    labels[3] = GTK_WIDGET(gtk_builder_get_object(builder, "l4"));
    labels[4] = GTK_WIDGET(gtk_builder_get_object(builder, "l5"));
    gtk_style_context_add_provider(gtk_widget_get_style_context(labels[0]), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(labels[1]), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(labels[2]), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(labels[3]), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(labels[4]), styling, GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(root, "destroy", G_CALLBACK(quit_program), NULL);
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(root);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    for(i = 0; i < 5; i++){
        pthread_create(&threads[i], 0, &dining, &philosophers[i]);
    }
    
    gtk_widget_show(root);
    gtk_main();

    for(i = 0; i < 5; i++){
        pthread_join(threads[i], 0);
    }
    for(i = 0; i < 5; i++){
        sem_destroy(&chopsticks[i]);
    }
    sem_destroy(&pick);
    return 0;
}