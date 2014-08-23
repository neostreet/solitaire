#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAIN_MODULE
#include "solitaire.h"

static char usage[] = "solitaire_deck (-debug) (-rank_only) (-integer) seed\n";

static void initialize_deck(struct pile *deck);
static void shuffle_deck(struct pile *deck);
static void print_pile(struct pile *pile,bool bRankOnly,bool bInteger);
static void print_card(char card,char delim,bool bRankOnly,bool bInteger);
static void deal_to_piles(struct pile *deck,struct pile *piles);
static void print_piles(struct pile *piles,bool bOnlyFaceUp,bool bRankOnly,bool bInteger);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bRankOnly;
  bool bInteger;
  int seed;
  struct pile deck;
  struct pile piles[NUM_PILES];
  int retval;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bRankOnly = false;
  bInteger = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-rank_only"))
      bRankOnly = true;
    else if (!strcmp(argv[curr_arg],"-integer"))
      bInteger = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&seed);

  srand(seed);

  initialize_deck(&deck);

  if (bDebug) {
    printf("print_pile(&deck,bRankOnly,bInteger);\n");
    print_pile(&deck,bRankOnly,bInteger);
  }

  shuffle_deck(&deck);
  deal_to_piles(&deck,piles);
  print_piles(piles,false,bRankOnly,bInteger);

  return 0;
}

static void initialize_deck(struct pile *deck)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++)
    deck->pile[n] = n;

  deck->pile_depth = NUM_CARDS_IN_DECK;
}

static void shuffle_deck(struct pile *deck)
{
  int n;
  int work;
  char temp;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    work = rand() % NUM_CARDS_IN_DECK;

    if (work != n) {
      temp = deck->pile[work];
      deck->pile[work] = deck->pile[n];
      deck->pile[n] = temp;
    }
  }
}

static void print_pile(struct pile *pile,bool bRankOnly,bool bInteger)
{
  int n;

  for (n = 0; n < pile->pile_depth; n++)
    print_card(pile->pile[n],0x0a,bRankOnly,bInteger);
}

static void print_card(char card,char delim,bool bRankOnly,bool bInteger)
{
  int suit;
  int rank;

  if (bInteger)
    printf("%d%c",card,delim);
  else {
    if (!bRankOnly)
      suit = suit_of(card);

    rank = rank_of(card);

    if (!bRankOnly)
      printf("%c%c%c",rank_chars[rank],suit_chars[suit],delim);
    else
      printf("%c%c",rank_chars[rank],delim);
  }
}

static void deal_to_piles(struct pile *deck,struct pile *piles)
{
  int m;
  int n;
  int p;

  for (n = 0; n < NUM_PILES; n++) {
    piles[n].pile_depth = n + 1;
    piles[n].face_up_depth = 1;
  }

  p = 0;

  for (m = 0; m < NUM_PILES; m++) {
    for (n = m; n < NUM_PILES; n++) {
      piles[n].pile[piles[n].pile_depth - 1 - m] = deck->pile[p++];
    }
  }

  for (n = 0; n < NUM_CARDS_IN_DECK - p; n++)
    deck->pile[n] = deck->pile[n + p];

  deck->pile_depth = NUM_CARDS_IN_DECK - p;
}

static void print_piles(struct pile *piles,bool bOnlyFaceUp,bool bRankOnly,bool bInteger)
{
  int m;
  int n;

  for (m = 0; m < NUM_PILES; m++) {
    if (bOnlyFaceUp) {
      for (n = 0; n < piles[m].face_up_depth; n++) {
        if (n < piles[m].face_up_depth - 1)
          print_card(piles[m].pile[n],' ',bRankOnly,bInteger);
        else
          print_card(piles[m].pile[n],0x0a,bRankOnly,bInteger);
      }
    }
    else {
      for (n = 0; n < piles[m].pile_depth; n++) {
        if (n < piles[m].pile_depth - 1)
          print_card(piles[m].pile[n],' ',bRankOnly,bInteger);
        else
          print_card(piles[m].pile[n],0x0a,bRankOnly,bInteger);
      }
    }
  }
}
