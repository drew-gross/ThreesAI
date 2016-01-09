while true
do
	git pull
	pod install
	xcodebuild -workspace ThreesAI.xcworkspace -scheme "Release ThreesAI"
	./build/Products/Release/ThreesAI
done
