import os
import speech_recognition as sr
from google.cloud import speech_v1p1beta1 as speech

# Set up Google Cloud credentials
os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = 'replace_your_json_file_here.json'

def transcribe_audio(audio_data, language_code='en-US'):
    client = speech.SpeechClient()

    audio = speech.RecognitionAudio(content=audio_data)

    config = speech.RecognitionConfig(
        encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
        sample_rate_hertz=44100,
        language_code=language_code,
    )

    response = client.recognize(config=config, audio=audio)

    for result in response.results:
        print("Transcript: {}".format(result.alternatives[0].transcript))

def main():
    recognizer = sr.Recognizer()
    microphone = sr.Microphone()

    print("Listening...")

    with microphone as source:
        recognizer.adjust_for_ambient_noise(source)
        audio_data = recognizer.listen(source)

    print("Recognizing...")

    try:
        transcribe_audio(audio_data.frame_data)
    except Exception as e:
        print("An error occurred:", str(e))

if __name__ == "__main__":
    main()
