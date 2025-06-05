#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define LINESIZE 128

typedef struct Car Car;
typedef struct Heap Heap;

struct Car {
    char* brand;
    char* model;
    int year;
    int price;
};

struct Heap {
    Car* cars;
    int size;
};

void swapCar(Car* c1, Car* c2) {
    Car aux = *c1;
    *c1 = *c2;
    *c2 = aux;
}

Car initCar(char* brand, char* model, int year, int price) {
    Car car = { .year = year, .price = price };
    car.brand = malloc((strlen(brand) + 1) * sizeof(char));
    strcpy(car.brand, brand);
    car.model = malloc((strlen(model) + 1) * sizeof(char));
    strcpy(car.model, model);
    return car;
}

void printCarToConsole(Car car) {
    printf("%s %s (%d) - %d EUR\n", car.brand, car.model, car.year, car.price);
}

void freeCar(Car car) {
    free(car.brand);
    free(car.model);
}

Heap initHeap() {
    Heap heap;
    heap.size = 0;
    heap.cars = malloc(heap.size * sizeof(Car));
    return heap;
}

void insertHeap(Heap* heap, Car car) {
    heap->size += 1;
    heap->cars = realloc(heap->cars, heap->size * sizeof(Car));
    int element_index = heap->size - 1;
    heap->cars[element_index] = car;
    int parent_index = (element_index - 1) / 2;

    while (element_index > 0 && heap->cars[element_index].price > heap->cars[parent_index].price) {
        swapCar(&heap->cars[element_index], &heap->cars[parent_index]);
        element_index = parent_index;
        parent_index = (element_index - 1) / 2;
    }
}

void printHeap(Heap heap) {
    printf("Heap (prices): ");
    for (int i = 0; i < heap.size; i++) {
        printf("%d ", heap.cars[i].price);
    }
    printf("\n");
}

void printAllCars(Heap heap) {
    printf("All cars in heap:\n");
    for (int i = 0; i < heap.size; i++) {
        printf("[%d] ", i);
        printCarToConsole(heap.cars[i]);
    }
    printf("\n");
}

void heapify(Heap heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap.size && heap.cars[left].price > heap.cars[largest].price) {
        largest = left;
    }
    if (right < heap.size && heap.cars[right].price > heap.cars[largest].price) {
        largest = right;
    }
    if (largest != index) {
        swapCar(&heap.cars[largest], &heap.cars[index]);
        heapify(heap, largest);
    }
}

Heap buildHeap(Car* cars, int size) {
    Heap heap;
    heap.size = size;
    heap.cars = malloc(size * sizeof(Car));

    for (int i = 0; i < size; i++) {
        heap.cars[i] = initCar(cars[i].brand, cars[i].model, cars[i].year, cars[i].price);
    }

    int startIndex = heap.size / 2 - 1;
    for (int i = startIndex; i >= 0; i--) {
        heapify(heap, i);
    }
    return heap;
}

Car deleteFromHeap(Heap* heap) {
    if (heap->size == 0) {
        Car emptyCar = { 0 };
        return emptyCar;
    }

    Car car = heap->cars[0];
    swapCar(&heap->cars[0], &heap->cars[heap->size - 1]);

    heap->size -= 1;
    heap->cars = realloc(heap->cars, heap->size * sizeof(Car));

    if (heap->size > 0) {
        heapify(*heap, 0);
    }
    return car;
}

int deleteCarsByCriteria(Heap* heap, int (*criteria)(Car)) {
    int deletedCount = 0;
    int i = 0;

    while (i < heap->size) {
        if (criteria(heap->cars[i])) {
            freeCar(heap->cars[i]);
            heap->cars[i] = heap->cars[heap->size - 1];
            heap->size--;
            deletedCount++;
            heap->cars = realloc(heap->cars, heap->size * sizeof(Car));
        }
        else {
            i++;
        }
    }

    if (heap->size > 0) {
        int startIndex = heap->size / 2 - 1;
        for (int j = startIndex; j >= 0; j--) {
            heapify(*heap, j);
        }
    }

    return deletedCount;
}

int isOldCar(Car car) {
    return car.year < 2010;
}

int isExpensiveCar(Car car) {
    return car.price > 50000;
}

int isBMW(Car car) {
    return strcmp(car.brand, "BMW") == 0;
}

Car readCarFromFile(FILE* f) {
    Car car;
    char buffer[LINESIZE];
    if (fgets(buffer, LINESIZE, f) == NULL) {
        Car emptyCar = { 0 };
        return emptyCar;
    }

    buffer[strcspn(buffer, "\n")] = 0;

    char* token = strtok(buffer, ",");
    if (token == NULL) {
        Car emptyCar = { 0 };
        return emptyCar;
    }

    car.brand = malloc((strlen(token) + 1) * sizeof(char));
    strcpy(car.brand, token);

    token = strtok(NULL, ",");
    if (token == NULL) {
        free(car.brand);
        Car emptyCar = { 0 };
        return emptyCar;
    }
    car.model = malloc((strlen(token) + 1) * sizeof(char));
    strcpy(car.model, token);

    token = strtok(NULL, ",");
    if (token == NULL) {
        free(car.brand);
        free(car.model);
        Car emptyCar = { 0 };
        return emptyCar;
    }
    car.year = atoi(token);

    token = strtok(NULL, ",");
    if (token == NULL) {
        free(car.brand);
        free(car.model);
        Car emptyCar = { 0 };
        return emptyCar;
    }
    car.price = atoi(token);

    return car;
}

void readCarsFromFile(const char* filename, Heap* heap) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    while (!feof(f)) {
        Car car = readCarFromFile(f);
        if (car.brand != NULL) {
            insertHeap(heap, car);
        }
    }
    fclose(f);
}

void freeHeap(Heap* heap) {
    for (int i = 0; i < heap->size; i++) {
        freeCar(heap->cars[i]);
    }
    free(heap->cars);
    heap->size = 0;
}

int main() {
    printf("\n=== Test read from file ===\n");

    Heap heap = initHeap();
    readCarsFromFile("cars.txt", &heap);
    printf("Heap after reading from file:\n");
    printHeap(heap);
    printAllCars(heap);

    printf("\n=== Test manual insert ===\n");
    insertHeap(&heap, initCar("Tesla", "Model S", 2023, 95000));
    printf("After inserting Tesla Model S:\n");
    printHeap(heap);

    printf("\n=== Test delete by criteria: old cars (< 2010) ===\n");
    int deletedOld = deleteCarsByCriteria(&heap, isOldCar);
    printf("Deleted %d old cars.\n", deletedOld);
    printHeap(heap);
    printAllCars(heap);

    printf("\n=== Test delete by criteria: BMW cars ===\n");
    int deletedBMW = deleteCarsByCriteria(&heap, isBMW);
    printf("Deleted %d BMW cars.\n", deletedBMW);
    printHeap(heap);
    printAllCars(heap);

    printf("\n=== Test delete by criteria: expensive cars (> 50000) ===\n");
    int deletedExpensive = deleteCarsByCriteria(&heap, isExpensiveCar);
    printf("Deleted %d expensive cars.\n", deletedExpensive);
    printHeap(heap);
    printAllCars(heap);

    printf("\n=== Test normal delete from heap ===\n");
    while (heap.size > 0) {
        Car deletedCar = deleteFromHeap(&heap);
        printf("Deleted: ");
        printCarToConsole(deletedCar);
        freeCar(deletedCar);
    }

    printf("\n=== Test with car array ===\n");
    Car testCars[5] = {
        {0}, {0}, {0}, {0}, {0}
    };

    testCars[0] = initCar("Ford", "Focus", 2018, 15000);
    testCars[1] = initCar("Audi", "A4", 2020, 35000);
    testCars[2] = initCar("Mercedes", "C-Class", 2019, 40000);
    testCars[3] = initCar("Volkswagen", "Golf", 2017, 18000);
    testCars[4] = initCar("Porsche", "911", 2021, 120000);

    Heap heap2 = buildHeap(testCars, 5);
    printf("Heap built from array:\n");
    printHeap(heap2);
    printAllCars(heap2);

    for (int i = 0; i < 5; i++) {
        freeCar(testCars[i]);
    }

    printf("\n=== Test delete expensive cars from heap2 ===\n");
    int deletedExpensive2 = deleteCarsByCriteria(&heap2, isExpensiveCar);
    printf("Deleted %d expensive cars.\n", deletedExpensive2);
    printHeap(heap2);
    printAllCars(heap2);

    freeHeap(&heap);
    freeHeap(&heap2);

    return 0;
}