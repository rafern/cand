/* CAND: Mini sand simulation in C */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* cand context */
struct cactx
{
    size_t w; /* Width         */
    size_t h; /* Height        */
    int*  tm; /* Tile map      */
    char* pb; /* Print buffer  */
};

/* Prototypes */
int  initca (struct cactx*);
void cleanup(struct cactx*);
void setTile(struct cactx*, size_t, size_t, int);
int  getTile(struct cactx*, size_t, size_t);
void setChar(struct cactx*, size_t, size_t, char);
int  getMask(struct cactx*, size_t, size_t);
void genmap (struct cactx*);
void update (struct cactx*);
void render (struct cactx*);

/* Initialize cand context by allocating memory */
int initca(struct cactx* ctx)
{
    size_t i;
    ctx->tm = malloc(ctx->w * ctx->h * sizeof(int));
    ctx->pb = malloc((ctx->w + 1) * ctx->h + 1);

    if(ctx->tm && ctx->pb)
    {
        memset(ctx->tm, 0, ctx->w * ctx->h * sizeof(int));

        for(i = 0; i < ctx->h; ++i)
            ctx->pb[(ctx->w + 1) * i + ctx->w] = '\n';
        ctx->pb[(ctx->w + 1) * ctx->h] = '\0';
    }

    return ctx->tm && ctx->pb;
}

/* Cleanup memory in ascii2o context */
void cleanup(struct cactx* ctx)
{
    free(ctx->tm);
    free(ctx->pb);
    ctx->tm  = NULL;
    ctx->pb  = NULL;
}

/* Set tile type at position */
void setTile(struct cactx* ctx, size_t x, size_t y, int type)
{
    ctx->tm[y * ctx->w + x] = type;
}

/* Get tile type at position */
int getTile(struct cactx* ctx, size_t x, size_t y)
{
    return ctx->tm[y * ctx->w + x];
}

/* Set character at position in print buffer */
void setChar(struct cactx* ctx, size_t x, size_t y, char c)
{
    ctx->pb[(ctx->w + 1) * y + x] = c;
}

/* Get bitmask of tile at given position */
int getMask(struct cactx* ctx, size_t x, size_t y)
{
    int t = getTile(ctx, x, y);
    int m = 0;

    if(y > 0 && getTile(ctx, x, y - 1) == t)
        m += 1;
    if(x < ctx->w - 1 && getTile(ctx, x + 1, y) == t)
        m += 2;
    if(y < ctx->h - 1 && getTile(ctx, x, y + 1) == t)
        m += 4;
    if(x > 0 && getTile(ctx, x - 1, y) == t)
        m += 8;

    return m;
}

/* Update tiles */
void update(struct cactx* ctx)
{
    size_t x;
    size_t y;

    /* Bottom up for better gravity simulation */
    for(y = ctx->h; y-- > 0;)
    {
        for(x = 0; x < ctx->w; ++x)
        {
            if(getTile(ctx, x, y) != 2)
                continue;

            if(y + 1 < ctx->h)
            {
                /* Down */
                if(getTile(ctx, x, y + 1) == 0)
                {
                    setTile(ctx, x, y    , 0);
                    setTile(ctx, x, y + 1, 2);
                }
                /* Left-down */
                else if(x > 0 && getTile(ctx, x - 1, y + 1) == 0)
                {
                    setTile(ctx, x    , y    , 0);
                    setTile(ctx, x - 1, y + 1, 2);
                }
                /* Right-down */
                else if(x + 1 < ctx->w && getTile(ctx, x + 1, y + 1) == 0)
                {
                    setTile(ctx, x    , y    , 0);
                    setTile(ctx, x + 1, y + 1, 2);
                }
            }
        }
    }
}

/* Render tiles */
void render(struct cactx* ctx)
{
    char stonebm[17] = "*U<\\-H/|>/=_\\|\"`";
    char sandbm [17] = "oU(\\.|/|)/=_\\|~ " ;
    size_t x;
    size_t y;

    for(y = 0; y < ctx->h; ++y)
    {
        for(x = 0; x < ctx->w; ++x)
        {
            switch(getTile(ctx, x, y))
            {
            case 1:
                /* Stone */
                setChar(ctx, x, y, stonebm[getMask(ctx, x, y)]);
                break;
            case 2:
            {
                /* Sand */
                setChar(ctx, x, y, sandbm[getMask(ctx, x, y)]);
                break;
            }
            default:
                /* Air/Something else? */
                setChar(ctx, x, y, ' ');
                break;
            }
        }
    }

    /* Move to top left and print buffer contents */
    printf("\033[;H%s", ctx->pb);
}

int main(void)
{
    struct cactx ctx = {170, 49, NULL, NULL};
    struct timespec fTime;
    fTime.tv_sec  = 0;
    /* ~20 FPS max */
    fTime.tv_nsec = 50000000;
    srand((unsigned int)time(NULL));

    if(initca(&ctx))
    {
        int i;

        /* Clear screen */
        fputs("\033[2J", stdout);

        /* Add stone stone to spice things up */
        for(i = 0; i < 20 + rand() % 10; ++i)
        {
            /* If wall is grated or not */
            int g = rand() % 2;

            if(rand() % 2)
            {
                /* Vertical wall */
                size_t x  = (size_t)rand() % ctx.w;
                size_t y1 = (size_t)rand() % ctx.h;
                size_t y2 = (y1 + 6 + (rand() % 10)) % ctx.h;
                size_t y;

                for(y = y1; y < y2; ++y)
                {
                    if(g)
                        setTile(&ctx, x, y, y % 2);
                    else
                        setTile(&ctx, x, y, 1    );
                }
            }
            else
            {
                /* Horizontal wall */
                size_t y  = (size_t)rand() % ctx.h;
                size_t x1 = (size_t)rand() % ctx.w;
                size_t x2 = (x1 + 6 + (rand() % 10)) % ctx.w;
                size_t x;

                for(x = x1; x < x2; ++x)
                {
                    if(g)
                        setTile(&ctx, x, y, x % 2);
                    else
                        setTile(&ctx, x, y, 1    );
                }
            }
        }

        while(1)
        {
            /* Add falling sand and make holes in sand (25% chance) */
            if(rand() % 4 == 0)
            {
                for(i = 0; i <= 2; i += 2)
                {
                    size_t x;
                    size_t y;
                    size_t x1 = (size_t)rand() % ctx.w;
                    size_t y1 = (size_t)rand() % ctx.h;
                    size_t x2 = x1 + 4 + (size_t)rand() % 6;
                    size_t y2 = y1 + 4 + (size_t)rand() % 6;

                    for(x = x1; x < ctx.w && x < x2; ++x)
                    {
                        for(y = y1; y < ctx.h && y < y2; ++y)
                        {
                            if     (i == 0 && getTile(&ctx, x, y) == 2)
                                setTile(&ctx, x, y, 0);
                            else if(i == 2 && getTile(&ctx, x, y) == 0)
                                setTile(&ctx, x, y, 2);
                        }
                    }
                }
            }

            update(&ctx);
            render(&ctx);
            nanosleep(&fTime, NULL);
        }
    }

    cleanup(&ctx);
    return EXIT_SUCCESS;
}
