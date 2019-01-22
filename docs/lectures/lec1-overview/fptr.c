#include <stdio.h>
#include <stdlib.h>

// How might we build an "abstract" interface in C?

typedef enum {
  fish, 
  chicken
} food;

void cat_speak(void) {
  printf("Meow!\n");
}

int cat_eat(food f) {
  if (f == fish) return 1;
  else return 0;
}

void dog_speak(void) {
  printf("Woof!\n");
}

int dog_eat(food f) {
  if (f == chicken) return 1;
  else return 0;
}

int fish_eat(food f) {
  if (f == chicken) return 1;
  else return 0;
}

void fish_swim() {
  printf("Swim!\n");
}

typedef struct {
  int (*eat)(food);
  union {
    void (*speak)();
    void (*swim)();
  } u;
} animal;

animal* new_dog() {
  animal *a = malloc(sizeof(animal));
  a->u.speak = dog_speak;
  a->eat = dog_eat;
  return a;
}

animal* new_cat() {
  animal *a = malloc(sizeof(animal));
  a->u.speak = cat_speak;
  a->eat = cat_eat;
  return a;
}

animal* new_fish() {
  animal *a = malloc(sizeof(animal));
  a->u.swim = fish_swim;
  a->eat = fish_eat;
  return a;
}


void speaker(void (*speak)(void)) {
  speak();
}

int main(int argc, char **argv) {
  animal *cat = new_cat();
  animal *dog = new_dog();
  animal *fish = new_fish();

  cat->u.speak();
  dog->u.speak();
  fish->u.swim();

  printf("%d\n", cat->eat(chicken));
  printf("%d\n", dog->eat(chicken));
  printf("%d\n", fish->eat(chicken));

  fish->u.speak();

  speaker(cat_speak);
  speaker(dog_speak);

  return 0;
}
