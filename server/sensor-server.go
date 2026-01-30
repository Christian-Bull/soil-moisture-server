package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
)

func main() {
	port := os.Getenv("PORT")
	if port == "" {
		port = "5001"
	}

	l := log.New(os.Stdout, "soil-sensor-api", log.LstdFlags)

	bindAddr := fmt.Sprintf(":%s", port)

	hh := newSoilSensor(l)
	http.Handle("/", hh)

	l.Printf("Starting server on port %s", port)
	l.Fatal(http.ListenAndServe(bindAddr, nil))
}

type SoilSensor struct {
	l *log.Logger
}

func newSoilSensor(l *log.Logger) *SoilSensor {
	return &SoilSensor{l}
}

func (c *SoilSensor) ServeHTTP(rw http.ResponseWriter, r *http.Request) {
	fileName := os.Getenv("SOIL_SENSOR_FILENAME")
	if fileName == "" {
		fileName = "/var/tmp/raw_current"
	}

	data, err := os.ReadFile(fileName)
	if err != nil {
		log.Fatal(err)
	}
	content := string(data)

	fmt.Fprint(rw, content)
	c.l.Printf("Sent back raw sensor value")
}
