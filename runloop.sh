while true
do
	git pull
	pod install
	xcodebuild -workspace ThreesAI.xcworkspace -scheme "Release ThreesAI" > /dev/null
	./build/Products/Release/ThreesAI
done
