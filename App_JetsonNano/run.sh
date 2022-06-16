echo "Initializing..."
echo "Creating directories..."

mkdir -p /home/ect/Documentos/jetson-inference/build/aarch64/bin/processed
mkdir -p /home/ect/Documentos/jetson-inference/build/aarch64/bin/blured

echo "Image: airplane_0.jpg"
sudo python3 detectnet.py images/airplane_0.jpg processed/airplane_0.jpg
sudo python3 detectnetDataPrivacy.py images/airplane_0.jpg blured/airplane_0.jpg

echo "Image: dog_0.jpg"
sudo python3 detectnet.py images/dog_0.jpg processed/dog_0.jpg
sudo python3 detectnetDataPrivacy.py images/dog_0.jpg blured/dog_0.jpg

echo "Image: humans_1.jpg"
sudo python3 detectnet.py images/humans_1.jpg processed/humans_1.jpg
sudo python3 detectnetDataPrivacy.py images/humans_1.jpg blured/humans_1.jpg

echo "Image: humans_7.jpg"
sudo python3 detectnet.py images/humans_7.jpg processed/humans_7.jpg
sudo python3 detectnetDataPrivacy.py images/humans_7.jpg blured/humans_7.jpg

echo "Exiting..."
