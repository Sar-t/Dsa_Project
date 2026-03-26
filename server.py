from flask import Flask, request, jsonify
from sentence_transformers import SentenceTransformer
from sklearn.metrics.pairwise import cosine_similarity

app = Flask(__name__)

# Load AI model (only once)
model = SentenceTransformer('all-MiniLM-L6-v2')

@app.route('/search', methods=['POST'])
def search():
    data = request.json

    query = data['query']
    files = data['files']  # list of {path, content}

    # Extract file contents
    texts = [f['content'] for f in files]

    if not texts:
        return jsonify([])

    # Convert to embeddings
    file_embeddings = model.encode(texts)
    query_embedding = model.encode([query])

    # Compute similarity
    scores = cosine_similarity(query_embedding, file_embeddings)[0]

    results = []
    for i, score in enumerate(scores):
        results.append({
            "path": files[i]['path'],
            "score": float(score)
        })

    # Sort by best match
    results.sort(key=lambda x: x["score"], reverse=True)

    return jsonify(results)


if __name__ == "__main__":
    app.run(port=5000)