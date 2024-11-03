import { useEffect, useState } from "react";

const CardDataDisplay = () => {
  const [cardData, setCardData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchCardData = async () => {
      try {
        const response = await fetch("http://localhost:3001/cardData");
        if (!response.ok) {
          throw new Error("Network response was not ok");
        }
        const data = await response.json();
        setCardData(data);
      } catch (error) {
        setError(error.message);
      } finally {
        setLoading(false);
      }
    };

    fetchCardData();
  }, []);

  if (loading) return <p className="text-center mt-10 text-lg">Loading...</p>;
  if (error)
    return <p className="text-center mt-10 text-red-500">Error: {error}</p>;

  return (
    <div className="flex flex-col items-center py-10 bg-gray-100 min-h-screen">
      <h1 className="text-3xl font-bold mb-5">NFC Card Data</h1>
      <div className="bg-white shadow-lg rounded-lg p-6 w-full max-w-lg">
        <div className="mb-4">
          <h2 className="text-lg font-semibold">
            UID: <span className="font-normal">{cardData.UID}</span>
          </h2>
          <h2 className="text-lg font-semibold">
            SAK: <span className="font-normal">{cardData.SAK}</span>
          </h2>
          <h2 className="text-lg font-semibold">
            PICC Type: <span className="font-normal">{cardData.PICCType}</span>
          </h2>
        </div>
        <h3 className="text-2xl font-bold mt-4 mb-4">Sectors</h3>
        <div className="grid grid-cols-2 gap-4">
          {cardData.Sectors.map((sector) => (
            <div
              key={sector.Block}
              className="border rounded-lg p-4 shadow-md bg-gray-50"
            >
              <h4 className="text-lg font-semibold">Block {sector.Block}</h4>
              <p className="text-sm text-gray-700 mt-1">Data: {sector.Data}</p>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default CardDataDisplay;
