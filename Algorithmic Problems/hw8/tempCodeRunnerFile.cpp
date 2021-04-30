
                to.first--;

                if (from.second < from.first) {
                    printf("Break\n");
                    break;
                }
            }
            L++;
        }
        L = 1;
    }
    printf("End\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    
    
    for (int i = 0; i < N; i++) {
        free(cost[i]);
        free(result[i]);
    }
    free(cost);
    free(result);


    return 0;
}


