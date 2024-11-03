const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const express = require("express");
const cors = require("cors");

const app = express();
const PORT = process.env.PORT || 3001;

app.use(cors());

const port = new SerialPort({ path: "COM3", baudRate: 9600 });
const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));

let cardData = {
  UID: "",
  SAK: "",
  PICCType: "",
  Sectors: [],
};

parser.on("data", (data) => {
  console.log("Received:", data);

  if (data.includes("Card UID:")) {
    cardData.UID = data.split(": ")[1].trim();
    cardData.Sectors = [];
  } else if (data.includes("Card SAK:")) {
    cardData.SAK = data.split(": ")[1].trim();
  } else if (data.includes("PICC type:")) {
    cardData.PICCType = data.split(": ")[1].trim();
  } else if (data.includes("Block")) {
    // Parse each block's data
    const parts = data.split(/\s+/);
    const blockNumber = parseInt(parts[1].replace(":", ""), 10);

    if (!isNaN(blockNumber) && blockNumber >= 0 && blockNumber <= 63) {
      const blockData = parts.slice(2).join(" ").trim();
      cardData.Sectors.push({
        Block: blockNumber,
        Data: blockData,
      });
    }
  }
});

port.on("open", () => {
  console.log("Serial Port Opened");
});

port.on("error", (err) => {
  console.error("Error: ", err.message);
});

// endpoint
app.get("/cardData", (req, res) => {
  res.json(cardData);
});

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
